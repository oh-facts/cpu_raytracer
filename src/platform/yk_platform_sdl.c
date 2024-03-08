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

    //ToDo(facts): Use the asserts you made

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow(
        "yk",
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
    // ToDo(facts): use an arena
    u32 *buffer = malloc(win_surf->w * win_surf->h * sizeof(u32));

    struct YkGame game = {0};

    struct render_buffer render_target = {0};
    render_target.pixels = (u32 *)win_surf->pixels;
    render_target.width = win_surf->w;
    render_target.height = win_surf->h;

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
            }
            break;

            // ToDo(facts): Use a ternary operator or make an array of sdl_key_size and update
            // based on indices. I don't want to maintain two nearly identical
            // switch cases for input. I just put this together quick so I had something
            // to play with.
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
        render(&render_target, &game);
        
        SDL_UpdateWindowSurface(win);

  
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
    free(buffer);
    SDL_DestroyWindow(win);
    SDL_Quit();

#if mem_leak_msvc
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtDumpMemoryLeaks();
#endif
    return 0;
}