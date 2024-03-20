#include <yk_sdl2_include.h>
#include <miniaudio.h>
#include <yk_common.h>
#include <yk_game.h>

#include <sys/stat.h>
#include <time.h>

#define DEBUG_SDL_CHECK 1
#define LOG_STATS       0

#if DEBUG_SDL_CHECK
#define SDL_CHECK_RES(res) _Assert_helper(res == 0, "[SDL Assert is Failure]\n%s", SDL_GetError())
#define SDL_CHECK(expr) _Assert_helper(expr, "[SDL Assert is Failure]\n%s", SDL_GetError())
#else
#define SDL_CHECK_RES(expr) expr
#define SDL_CHECK(expr)
#endif


// ToDO: Haven't tested hot reloading on mac or linux.
#if     _WIN32
#define GAME_DLL        "yk2.dll"
#define GAME_DLL_CLONED "yk2_temp.dll"
#elif   __linux__
#define GAME_DLL        "libyk2.so"
#define GAME_DLL_CLONED "libyk2_temp.so"
#elif  __APPLE__        
#define GAME_DLL        "yk2.dylib"
#define GAME_DLL_CLONED "yk2_test.dylib"
#endif

internal char* yk_read_text_file(const char* filepath, struct Arena* arena);
internal int yk_clone_file(const char* sourcePath, const char* destinationPath);
internal char* yk_read_binary_file(const char* filename, struct Arena* arena);
internal time_t get_file_last_modified_time(const char* pathname);

struct sdl_platform
{
    yk_innit_game_func innit_game;
    yk_update_and_render_game_func update_and_render_game;
    void * game_dll;
    
    // unused
    time_t modified_time;
};

internal void load_game_dll(struct sdl_platform* platform)
{
    yk_clone_file(GAME_DLL, GAME_DLL_CLONED);
    
    platform->game_dll = SDL_LoadObject(GAME_DLL_CLONED);
    if (!platform->game_dll)
    {
        exit(432);
        
    }
    yk_innit_game_func innit_game = NULL;
    yk_update_and_render_game_func update_and_render_game = NULL;
    
    platform->innit_game = (yk_innit_game_func)SDL_LoadFunction(platform->game_dll, "yk_innit_game");
    platform->update_and_render_game = (yk_update_and_render_game_func)SDL_LoadFunction(platform->game_dll, "yk_update_and_render_game");
    platform->modified_time = get_file_last_modified_time(GAME_DLL);
}

internal void hot_reload(struct sdl_platform* platform)
{
    SDL_UnloadObject(platform->game_dll);
    load_game_dll(platform);
}

// sys/stat works on windows because its a compatibility thing. I don't know how safe this is. This function works as expected on my computer so it will stay. Also, this is a debug tool. I won't ship with this, so it will stay.
internal time_t get_file_last_modified_time(const char* pathname)
{
    struct stat stat_buf;
    if(stat(pathname,&stat_buf) == -1 )
    {
        printl("could not get last modified time. The dev needs to write more info here to make this error message remotely helpful");
        return 0;
    }
    
    return stat_buf.st_mtime;
}

//https://en.wikipedia.org/wiki/BMP_file_format
#pragma pack(push, 1)
struct BitmapHeader
{
    //file header
    u16 file_type;
    u32 file_size;
    u16 _reserved;
    u16 _reserved2;
    u32 pixel_offset;
    
    //DIB header
    u32 header_size;
    i32 width;
    i32 height;
    u16 color_panes;
    u16 depth;// stored in bits
};
#pragma pack(pop)


struct bitmap
{
    u32* pixels;
    u32 width;
    u32 height;
};

// Note(facts): This is stored upside down. So I flip it (along x axis)
internal struct bitmap read_bitmap_file(const char* filepath, struct Arena* arena)
{
    u32 * out = 0;
    
    size_t arena_save = arena->used;
    char* file_data = yk_read_binary_file(filepath, arena);
    
    struct BitmapHeader* header = (struct BitmapHeader*)file_data;
    AssertM(header->depth == 32, "your bitmap must use 32 bytes for each pixel");
    
    struct bitmap result;
    result.width = header->width;
    result.height = header->height;
    
    arena->used = arena_save;
    
    result.pixels = push_array(arena,u32,result.width*result.height);
    
    memcpy(result.pixels, (u8*)file_data + header->pixel_offset,result.width * result.height * sizeof(u32));
    
    // erm, there has to be a better way to flip pixels right?
    u32 temp;
    for (size_t y = 0, yy = result.height; y < yy / 2; y++)
    {
        for (size_t x = 0, xx = result.width; x < xx ; x++)
        {
            temp = result.pixels[y * xx + x];
            result.pixels[y * xx + x] = result.pixels[(yy- 1 - y) * xx + x];
            result.pixels[(yy - 1 - y) * xx + x] = temp;
        }
    }
    
    
    return result;
}


#define GAME_UPDATE_RATE (1/60.f)

ma_result result;
ma_engine engine;


// ToDO(facts): Do properly loser
void sdl_play_audio(const char* path)
{
    ma_engine_play_sound(&engine, path, 0);
}

void sdl_set_title(void* win, const char* title)
{
    // incase text doesn't fully display
    printf("%s\n",title);
    SDL_SetWindowTitle(win, title);
}

int main(int argc, char *argv[])
{
    
    // platform shit starts here ------------------------
    struct sdl_platform platform = {0};
    load_game_dll(&platform);
    
    f64 total_time_elapsed = 0;
    f64 dt = 0;
    
    SDL_CHECK_RES(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO));
    
    if(TTF_Init() != 0)
    {
        printf("%s",SDL_GetError());
    }
    
    SDL_Window *win = SDL_CreateWindow(
                                       "television",
                                       SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600,
                                       0);
    
    SDL_CHECK(win);
    
    
    result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        return -1;
    }
    
    SDL_Surface *win_surf = SDL_GetWindowSurface(win);
    SDL_CHECK(win_surf);
    
    SDL_Event event;
    b8 quit = 0;
    
    f32 a_ratio = 8 / 6.f;
    
    
    //-----------------------------platform shit ends here
    
    // game boilerplate
    struct YkInput input = {0};
    
    struct YkGame game = {0};
    
    size_t mem_size = Megabytes(10);
    arena_innit(&game.arena,mem_size,malloc(mem_size));
    //platform
    game._win = win;
    game.platform_play_audio = sdl_play_audio;
    game.platform_set_title = sdl_set_title;
    
    platform.innit_game(&game);
    
    struct render_buffer render_target = {0};
    // SDL_GetWindowSize(win, &render_target.height, &render_target.width);
    render_target.height = 60;
    render_target.width = 80;
    render_target.pixels = push_array(&game.arena, sizeof(u32),render_target.height * render_target.width);
    
    SDL_Surface *render_surface = SDL_CreateRGBSurfaceFrom(render_target.pixels, render_target.width, render_target.height, 32, render_target.width * sizeof(u32), 0xFF0000, 0xFF00, 0xFF, 0xFF000000);
    
    TTF_Font* font = TTF_OpenFont("../res/Delius-Regular.ttf",48);
    
    if(!font)
    {
        printf("%s",SDL_GetError());
    }
    SDL_Surface* text = TTF_RenderText_Solid(font, "dear dear", (SDL_Color){255,0,0});
    SDL_Surface* text2 = TTF_RenderText_Solid(font, "loves you", (SDL_Color){255,0,0});
    if(!text)
    {
        printf("%s",SDL_GetError());
    }
    
    SDL_Rect dstRect;
    
    
    //SDL_BlitScaled(pixels, 0, render_surface, &dst);
    
    struct bitmap bmp = read_bitmap_file("../res/test.bmp",&game.arena);
    
#if 0
    //Dangerous! Only call if its a small image. You won't be able to ctrl + c out of this if its millions of pixels
    for(u32 y = 0; y < bmp.height; y ++)
    {
        for(u32 x = 0; x < bmp.width; x ++)
        {
            printf("%x ",bmp.pixels[y*bmp.width + x]);
        }
        printl("");
    }
#endif
    SDL_Surface *bmo = SDL_CreateRGBSurfaceFrom(bmp.pixels, bmp.width, bmp.height, 32, bmp.width * sizeof(u32), 0xFF0000, 0xFF00, 0xFF, 0xFF000000);
    /*
    dstRect.x = 22;
    dstRect.y = 0;
    dstRect.w = bmo->w / 30;
    dstRect.h = bmo->h / 30;
    
    
    SDL_BlitScaled(bmo,0, win_surf, &dstRect.x);
    
    SDL_UpdateWindowSurface(win);
    */
    SDL_DisplayMode dm;
    
    u8 isF = 0;
    
    while (!quit)
    {
        time_t modified_time = get_file_last_modified_time(GAME_DLL);
        
        if(modified_time > platform.modified_time)
        {
            hot_reload(&platform);
        }
        
        f64 last_time_elapsed = total_time_elapsed;
        
        // ToDo(facts): Move this into a function. I dont want to scroll so much
        
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                {
                    quit = 1;
                }break;
                
                case SDL_KEYUP:
                case SDL_KEYDOWN:
                {
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_UP:
                        case SDLK_w:
                        {
                            input.keys[YK_ACTION_UP] = event.type == SDL_KEYDOWN ? 1 : 0;
                        }break;
                        
                        case SDLK_LEFT:
                        case SDLK_a:
                        {
                            input.keys[YK_ACTION_LEFT] = event.type == SDL_KEYDOWN ? 1 : 0;
                        }break;
                        
                        case SDLK_DOWN:
                        case SDLK_s:
                        {
                            input.keys[YK_ACTION_DOWN] = event.type == SDL_KEYDOWN ? 1 : 0;
                        }break;
                        
                        case SDLK_RIGHT:
                        case SDLK_d:
                        {
                            input.keys[YK_ACTION_RIGHT] = event.type == SDL_KEYDOWN ? 1 : 0;
                        }break;
                        
                        case SDLK_F1:
                        {
                            input.keys[YK_ACTION_HOLD_HANDS] = event.type == SDL_KEYDOWN ? 1 : 0;
                        }break;
                        case SDLK_RETURN:
                        {
                            input.keys[YK_ACTION_ACCEPT] = event.type == SDL_KEYDOWN ? 1 : 0;
                        }break;
                        
                        case SDLK_F2:
                        {
                            if (event.type == SDL_KEYDOWN)
                            {
                                if (!isF)
                                {
                                    isF = 1;
                                    SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN_DESKTOP);
                                }
                                else
                                {
                                    isF = 0;
                                    SDL_SetWindowFullscreen(win, 0);
                                }
                            }
                        }break;
                        
                        case SDLK_q:
                        {
                            quit = 1;
                        }break;
                        
                        case SDLK_1:
                        {
                            input.keys[YK_ACTION_SAVE] = event.type == SDL_KEYDOWN ? 1 : 0;
                        }break;
                        
                        case SDLK_2:
                        {
                            input.keys[YK_ACTION_RESTORE] = event.type == SDL_KEYDOWN ? 1 : 0;
                        }break;
                        
                        case SDLK_3:
                        {
                            hot_reload(&platform);
                        }break;
                        
                        default:
                        {
                            
                        }break;
                    }
                    
                }break;
                
                case SDL_WINDOWEVENT:
                {
                    win_surf = SDL_GetWindowSurface(win);
                    SDL_CHECK(win_surf);
                    SDL_GetDesktopDisplayMode(0, &dm);
                }break;
            }
        }
        
        // ToDo(facts): store pitch inside render_buffer
        
        
        //game.font = text;
        
        // game loop start--------
        local_persist f32 fixed_dt;
        fixed_dt += dt;
        
        if (fixed_dt > 1 / 60.f)
        {
            //render_target.pixels[0] = 0xFFFF0000;
            
            platform.update_and_render_game(&render_target, &input, &game, GAME_UPDATE_RATE);
            fixed_dt = 0;
            
            //renderText("F", 20, 50);
            if(isF)
            {
                
                u32 dh = dm.h * 4/5.f;
                
                dstRect.x = dm.w / 2 - text->w/2;    
                dstRect.y = dh;    
                dstRect.w = text->w;
                dstRect.h = text->h;
                
                SDL_BlitSurface(text, 0, win_surf, &dstRect);
                printf("%s",SDL_GetError());
                
                dstRect.x = dm.w / 2 - text->w/2;  
                dstRect.y = dh + 2 + text->h;
                dstRect.w = text->w;
                dstRect.h = text->h;
                
                SDL_BlitSurface(text2, 0, win_surf, &dstRect);
                
                dstRect.x = 25;
                dstRect.y = 200;
                dstRect.w = bmo->w / 4;
                dstRect.h = bmo->h / 4;
                
                
                SDL_BlitScaled(bmo,0, win_surf, &dstRect);
                
                
                dstRect.x = win_surf->w - 25 - bmo->w/4;
                dstRect.y = 200;
                dstRect.w = bmo->w / 4;
                dstRect.h = bmo->h / 4;
                
                
                SDL_BlitScaled(bmo,0, win_surf, &dstRect);
                
                SDL_Rect dest = {0};
                //printl("%d %d",dm.w,dm.h);
                dest.w = dh * 4/3.f;
                dest.h = dh;
                dest.x = (dm.w - dest.w)/2 ;
                
                SDL_BlitScaled(render_surface, 0, win_surf, &dest);
                
                
            }
            else
            {
                SDL_BlitScaled(render_surface, 0, win_surf, 0);
            }
            
            // Might fail. Leaving it like this until it doesn't
            // It is possible that my window becomes invalid before
            // I recreate it. I could be wrong about this.
            // SDL_CHECK_RES(SDL_UpdateWindowSurface(win));
            
            // did fail. removed it.
            SDL_UpdateWindowSurface(win);
            
            for (u32 i = 0; i < YK_ACTION_COUNT; i++)
            {
                input.keys_old[i] = input.keys[i];
            }
        }
        
        //-------game loop end
        
        total_time_elapsed = SDL_GetTicks64() / 1000.f;
        
        dt = total_time_elapsed - last_time_elapsed;
        
        // perf stats
#if LOG_STATS
        
#define num_frames_for_avg 60
#define print_stats_time 5
        
        local_persist f64 frame_time;
        local_persist u32 frame_count;
        local_persist f32 time_since_print;
        
        frame_time += dt;
        frame_count++;
        time_since_print += dt;
        
        if (time_since_print > print_stats_time)
        {
            frame_time /= frame_count;
            f64 frame_rate = 1 / frame_time;
            
            printf("\n     perf stats     \n");
            printf("\n--------------------\n");
            
            printf("frame time : %.3f ms\n", frame_time * 1000.f);
            printf("frame rate : %.0f \n", frame_rate);
            
            printf("---------------------\n");
            
            frame_time = 0;
            frame_count = 0;
            
            time_since_print = 0;
        }
#endif
    }
    
#if mem_leak_msvc
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtDumpMemoryLeaks();
#endif
    return 0;
}

/*
fopen_s() is only defined in windows
*/
#ifdef __unix
#define fopen_s(pFile, filepath, mode) ((*(pFile)) = fopen((filepath), (mode))) == NULL
#endif
/*
Haven't tested it. Wrote it in a different engine. Modified it to use the arena
*/
internal char* yk_read_text_file(const char* filepath, struct Arena* arena)
{
    FILE* file;
    fopen_s(&file, filepath, "r");
    
    AssertM(file, "Unable to open the file %s\n",filepath);
    
    fseek(file, 0, SEEK_END);
    int length = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    AssertM(length > 0, "File %s is empty",filepath);
    
    char* string = push_array(arena, char, length + 1);
    
    char c;
    int i = 0;
    
    while ((c = fgetc(file)) != EOF)
    {
        string[i] = c;
        i++;
    }
    string[i] = '\0';
    
    fclose(file);
    
    return string;
}

// tested. works. will fail if dll is > 4kb. Currently it is 28 kb. (I am only using it for dll currently so I am using fat array).
internal int yk_clone_file(const char* sourcePath, const char* destinationPath)
{
    FILE* sourceFile, * destinationFile;
    char buffer[4096];
    size_t bytesRead;
    
    fopen_s(&sourceFile, sourcePath, "rb");
    if (sourceFile == NULL) {
        perror("Error opening source file");
        return 0;
    }
    
    fopen_s(&destinationFile, destinationPath, "wb");
    if (destinationFile == NULL) {
        perror("Error opening destination file");
        fclose(sourceFile);
        return 0;
    }
    
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), sourceFile)) > 0) {
        fwrite(buffer, 1, bytesRead, destinationFile);
    }
    
    fclose(sourceFile);
    fclose(destinationFile);
    
    return 1;
}

// Tested. Works.
internal char* yk_read_binary_file(const char* filename, struct Arena* arena)
{
    FILE* file;
    fopen_s(&file, filename, "rb");
    
    if (!file) {
        perror("Failed to open file");
        return NULL;
    }
    
    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* buffer = push_array(arena,u8,fileSize);
    
    if (fread(buffer, 1, fileSize, file) != fileSize)
    {
        perror("Failed to read file");
        arena->used -= fileSize;
        fclose(file);
        return 0;
    }
    
    fclose(file);
    
    return buffer;
}