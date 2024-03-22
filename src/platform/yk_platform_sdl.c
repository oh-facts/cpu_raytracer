#include <platform/yk_sdl2_include.h>
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

char* yk_read_binary_file(const char* filename, struct Arena* arena);

internal char* yk_read_text_file(const char* filepath, struct Arena* arena);
internal int yk_clone_file(const char* sourcePath, const char* destinationPath);
internal time_t get_file_last_modified_time(const char* pathname);

struct sdl_platform
{
    yk_innit_game_func innit_game;
    yk_update_and_render_game_func update_and_render_game;
    void * game_dll;
    
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

#define GAME_UPDATE_RATE (1/60.f)

// ToDo(facts): Do audio properly loser

ma_result result;
ma_engine engine;

ma_sound sounds[10];
u32 num_sounds;

u32 miniaudio_innit_audio(const char* path)
{
    result = ma_sound_init_from_file(&engine, path, 0, NULL, NULL, &sounds[num_sounds]);
    if (result != MA_SUCCESS)
    {
        printf("Audio %s not found",path);
    }
    return num_sounds++;
}

void miniaudio_play_audio(u32 id)
{
    ma_sound_start(&sounds[id]);
}

void miniaudio_stop_audio(u32 id)
{
    ma_sound_stop(&sounds[id]);
    ma_sound_seek_to_pcm_frame(&sounds[id], 0);
}

void sdl_set_title(void* win, const char* title)
{
    // incase text doesn't fully display
    printf("%s\n",title);
    SDL_SetWindowTitle(win, title);
}

#if _WIN32
#include <Windows.h>
#endif

int main(int argc, char *argv[])
{
    // FUCK MICROSOFT (John Malkovitch voice)
#if _WIN32
    SetProcessDPIAware();
#endif
    
    // platform shit starts here ------------------------
    struct sdl_platform platform = {0};
    load_game_dll(&platform);
    
    f64 total_time_elapsed = 0;
    f64 dt = 0;
    
    SDL_CHECK_RES(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO));
    
    const u32 tv_w  = 8;
    const u32 tv_h  = 6;
    const u32 pad_w = 4; // 4 on each side
    const u32 pad_h = 3;
    const u32 win_w = tv_w + 2 * pad_w;
    const u32 win_h = tv_h + pad_h;
    
    SDL_Window *win = SDL_CreateWindow(
                                       "television",
                                       SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 960, 540,
                                       SDL_WINDOW_RESIZABLE);
    
    SDL_CHECK(win);
    
    result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        return -1;
    }
    
    SDL_Surface *win_surf = SDL_GetWindowSurface(win);
    SDL_CHECK(win_surf);
    
    SDL_Event event;
    b8 quit = 0;
    
    //-----------------------------platform shit ends here
    
    // game boilerplate
    struct YkInput input = {0};
    
    struct YkGame game = {0};
    
    size_t mem_size = Megabytes(100);
    arena_innit(&game.arena,mem_size,malloc(mem_size));
    arena_innit(&game.scratch, mem_size,malloc(mem_size));
    
    //platform
    game._win = win;
    game.platform_innit_audio = miniaudio_innit_audio;
    game.platform_play_audio  = miniaudio_play_audio;
    game.platform_stop_audio  = miniaudio_stop_audio;
    game.platform_set_title   = sdl_set_title;
    game.platform_read_file   = yk_read_binary_file;
    
    
    struct render_buffer render_target = {0};
    render_target.height = win_surf->h;
    render_target.width = win_surf->w;
    render_target.pixels = win_surf->pixels;
    
    platform.innit_game(&game, &render_target);
    
    
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
    
    //SDL_Surface *bmo = SDL_CreateRGBSurfaceFrom(bmp.pixels, bmp.width, bmp.height, 32, bmp.width * sizeof(u32), 0xFF0000, 0xFF00, 0xFF, 0xFF000000);
    
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
                                    SDL_SetWindowSize(win,960,540);
                                    SDL_SetWindowPosition(win, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
                                    
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
                
                //ToDO:width resizing does not work
                case SDL_WINDOWEVENT:
                {
                    switch (event.window.event)
                    {
                        case SDL_WINDOWEVENT_RESIZED:
                        {
                            win_surf = SDL_GetWindowSurface(win);
                            
                            SDL_CHECK(win_surf);
                            SDL_GetDesktopDisplayMode(0, &dm);
                            
                            i32 width, height;
                            SDL_GetWindowSize(win, &width, &height);
                            
                            if(width < 800)
                            {
                                width = 800;
                                SDL_SetWindowSize(win, width, height);
                            }
                            
                            
                        }break;
                    }
                }break;
                
                
            }
        }
        
        // game loop start--------
        local_persist f32 fixed_dt;
        fixed_dt += dt;
        
        if (fixed_dt > 1 / 60.f)
        {
            fixed_dt = 0;
            
            win_surf = SDL_GetWindowSurface(win);
            
            render_target.pixels = win_surf->pixels;
            render_target.width = win_surf->w;
            render_target.height = win_surf->h;
            
            platform.update_and_render_game(&render_target, &input, &game, GAME_UPDATE_RATE);
            
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
char* yk_read_binary_file(const char* filename, struct Arena* arena)
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