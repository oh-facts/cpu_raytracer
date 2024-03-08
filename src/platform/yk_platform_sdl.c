#include <yk_platform.h>

#include <yk_sdl2_include.h>

#include <yk_common.h>
#include <yk_game.h>

int main(int argc, char *argv[])
{
    struct YkClockRaw clock_raw = {0};
    yk_clock_innit(&clock_raw);

    f64 total_time_elapsed = 0;
    f64 dt = 0;

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow(
        "michaelsoft bimbos",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600,
        0);

    if (!win)
    {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Main loop
    SDL_Event event;
    int quit = 0;

    struct YkInput input = {0};

    SDL_Surface *win_surf = SDL_GetWindowSurface(win);
    SDL_Renderer *ren = SDL_CreateSoftwareRenderer(win_surf);

    SDL_SetRenderDrawColor(ren, 0xFF, 0, 0xFF, 0xFF);

    u32 *buffer = malloc(win_surf->w * win_surf->h * sizeof(u32));

    struct YkGame game = {0};
    while (!quit)
    {
        f64 last_time_elapsed = total_time_elapsed;

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
            {
                quit = 1;
            }
            break;

            case SDL_KEYDOWN:
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_w:
                {
                    input.keys[YK_ACTION_UP] = 1;
                }
                break;
                case SDLK_a:
                {
                    input.keys[YK_ACTION_LEFT] = 1;
                }
                break;
                case SDLK_s:
                {
                    input.keys[YK_ACTION_DOWN] = 1;
                }
                break;
                case SDLK_d:
                {
                    input.keys[YK_ACTION_RIGHT] = 1;
                }
                break;

                default:
                    break;
                }
            }
            break;

            case SDL_KEYUP:
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_w:
                {
                    input.keys[YK_ACTION_UP] = 0;
                }
                break;
                case SDLK_a:
                {
                    input.keys[YK_ACTION_LEFT] = 0;
                }
                break;
                case SDLK_s:
                {
                    input.keys[YK_ACTION_DOWN] = 0;
                }
                break;
                case SDLK_d:
                {
                    input.keys[YK_ACTION_RIGHT] = 0;
                }
                break;

                default:
                    break;
                }
            }
            }
            break;
        }

        // game loop start--------

        handle_hand_holding(&input, &game);

        for (u32 i = 0; i < win_surf->w; i++)
        {
            for (u32 j = 0; j < win_surf->h; j++)
            {
                u32 posX = i - game.pos_x;
                u32 posY = j - game.pos_y;

                buffer[i + win_surf->w * j] = (0xFF << 24) | ((posX % 256) << 16) | ((0) << 8) | (posY % 256);
            }
        }

        SDL_Surface *image_surface = SDL_CreateRGBSurfaceFrom(buffer, win_surf->w, win_surf->h, 32, win_surf->w * sizeof(u32), 0xFF0000, 0x00FF00, 0x0000FF, 0xFF000000);
        SDL_BlitSurface(image_surface, 0, win_surf, 0);
        SDL_FreeSurface(image_surface);
        SDL_UpdateWindowSurface(win);

        SDL_SetRenderDrawColor(ren, 0, 0, 0, 0xFF);
        SDL_RenderClear(ren);

#if 0    
        if (yk_input_is_key_tapped(&state.window.keys, 'A'))
        {
            printf("A tapped");
        }


        if (yk_input_is_key_held(&state.window.keys, 'A'))
        {
            printf("A held");
        }

        if (yk_input_is_key_released(&state.window.keys, 'A'))
        {
            printf("A released");
        }

        if (yk_input_is_key_released(&state.window.keys, 'V'))
        {
            printf("V released");
        }

        if (yk_input_is_click(&state.window.clicks, YK_MOUSE_BUTTON_RIGHT))
        {
            printf("right Clicked");
        }
#endif
        //-------game loop end

        total_time_elapsed = yk_get_time_since(&clock_raw);

        dt = total_time_elapsed - last_time_elapsed;

        // perf stats
#if 1

#define num_frames_for_avg 60
#define print_stats_time 5

        local_persist f64 total_frame_rate;
        local_persist f64 total_frame_time;
        local_persist u32 frame_count;
        local_persist f32 time_since_print;

        total_frame_rate += 1 / dt;
        total_frame_time += dt;
        frame_count++;
        time_since_print += dt;

        if (time_since_print > print_stats_time)
        {
            f64 avg_frame_rate = total_frame_rate / frame_count;
            f64 avg_frame_time = total_frame_time / frame_count;

            printf("\n     perf stats     \n");
            printf("\n--------------------\n");

            // print instantaneous frame rate/time
#if 0
            printf("frame time : %.3f ms\n", dt * 1000.f);
            printf("frame rate : %.0f \n", 1/ dt);
#endif
            printf("average frame time : %.3f ms\n", avg_frame_time * 1000.f);
            printf("average frame rate : %.0f \n", avg_frame_rate);

            printf("---------------------\n");

            total_frame_rate = 0;
            total_frame_time = 0;
            frame_count = 0;

            time_since_print = 0;
        }
#endif
    }

    SDL_DestroyWindow(win);
    SDL_Quit();

#if mem_leak_msvc
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtDumpMemoryLeaks();
#endif
    return 0;
}