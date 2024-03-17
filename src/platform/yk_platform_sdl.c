#include <yk_sdl2_include.h>
#include <miniaudio.h>
#include <yk_common.h>
#include <yk_game.h>
#include <stb_truetype.h>

#define DEBUG_SDL_CHECK 1
#define LOG_STATS       0

#if DEBUG_SDL_CHECK
#define SDL_CHECK_RES(res) _Assert_helper(res == 0, "[SDL Assert is Failure]\n%s", SDL_GetError())
#define SDL_CHECK(expr) _Assert_helper(expr, "[SDL Assert is Failure]\n%s", SDL_GetError())
#else
#define SDL_CHECK_RES(expr) expr
#define SDL_CHECK(expr)
#endif

void render_bitmap(struct render_buffer *screen, u32 posx, u32 posy, u32 width, u32 height, u32 *bitmap)
{
    u32 minx = posx;
    u32 miny = posy;
    u32 maxx = posx + width;
    u32 maxy = posy + height;
    
    minx = (minx < 0) ? 0 : minx;
    miny = (miny < 0) ? 0 : miny;
    maxx = (maxx > screen->width) ? screen->width : maxx;
    maxy = (maxy > screen->height) ? screen->height : maxy;
    
    u8 *row = (u8 *)screen->pixels + miny * (screen->width * 4) + minx * 4;
    
    for (u32 y = miny; y < maxy; y++)
    {
        u32 *pixel = (u32 *)row;
        
        for (u32 x = minx; x < maxx; x++)
        {
            *pixel++ = *bitmap;
            bitmap += 4;
        }
        row += screen->width * 4;
    }
}




unsigned char* ttf_buffer;
u8* renderText() {
    
    FILE* font_file = fopen("../res/minecraft.ttf", "rb");
    fread(ttf_buffer, 1, 1<<20, font_file);
    fclose(font_file);
    
    stbtt_fontinfo font;
    stbtt_InitFont(&font, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer,0));
    
    int w,h,xo,yo;
    u8* bitmap = stbtt_GetCodepointBitmap(&font, 0,stbtt_ScaleForPixelHeight(&font, 20.f), 'N', &w, &h, &xo, &yo);
    
    u8* src = bitmap;
    u8* a = malloc(sizeof(u32) * w * h);
    u8* DestRow = a;
    for(u32 y = 0; y < h; y ++)
    {
        u32 *Dest = (u32*)DestRow;
        for(u32 x = 0; x < w; x ++)
        {
            u8 Alpha = *src++;
            *Dest++ = ((Alpha << 24) |
                       (Alpha << 16) |
                       (Alpha << 8)  |
                       (Alpha << 0) );
        }
        Dest += w * sizeof(u32);
    }
    
    stbtt_FreeBitmap(bitmap,0);
    
    return a;
}



#define GAME_UPDATE_RATE (1/60.f)

ma_result result;
ma_engine engine;

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
    ttf_buffer = malloc(1<<20);
    f64 total_time_elapsed = 0;
    f64 dt = 0;
    
    SDL_CHECK_RES(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO));
    
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
    
    //platform
    game._win = win;
    game.platform_play_audio = sdl_play_audio;
    game.platform_set_title = sdl_set_title;
    
    yk_innit_game(&game);
    
    struct render_buffer render_target = {0};
    // SDL_GetWindowSize(win, &render_target.height, &render_target.width);
    render_target.height = 60;
    render_target.width = 80;
    render_target.pixels = malloc(sizeof(u32) * render_target.height * render_target.width);
    
    SDL_Surface *render_surface = SDL_CreateRGBSurfaceFrom(render_target.pixels, render_target.width, render_target.height, 32, render_target.width * sizeof(u32), 0xFF0000, 0xFF00, 0xFF, 0xFF000000);
    u8* text = renderText();
    
    u32* pixels = SDL_LoadBMP("../res/test.bmp")->pixels;
    render_bitmap(&render_target,10,10,20,20,pixels);
    
    //--------------------
    // ugly fullscreen hack.
    // I dont want to deal with pixel scaling bs.
    u8 isF = 0;
    while (!quit)
    {
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
                                    SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN);
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
                        
                        
                        default:
                        {
                            
                        }break;
                    }
                    
                }break;
                
                case SDL_WINDOWEVENT:
                {
                    win_surf = SDL_GetWindowSurface(win);
                    SDL_CHECK(win_surf);
                }break;
            }
        }
        
        // ToDo(facts): store pitch inside render_buffer
        
        
        game.font = text;
        
        // game loop start--------
        local_persist f32 fixed_dt;
        fixed_dt += dt;
        
        if (fixed_dt > 1 / 60.f)
        {
            render_target.pixels[0] = 0xFFFF0000;
            
            //yk_update_and_render_game(&render_target, &input, &game, GAME_UPDATE_RATE);
            fixed_dt = 0;
            
            //renderText("F", 20, 50);
            
            SDL_BlitScaled(render_surface, 0, win_surf, 0);
            
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
