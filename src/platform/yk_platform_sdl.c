#include <yk_sdl2_include.h>

#include <yk_common.h>
#include <yk_game.h>

#define DEBUG_SDL_CHECK 1

#if DEBUG_SDL_CHECK
#define SDL_CHECK_RES(res) _Assert_helper(res == 0, "%s", SDL_GetError())
#define SDL_CHECK(expr) _Assert_helper(expr, "%s", SDL_GetError())
#else
#define SDL_CHECK_RES(expr) expr
#define SDL_CHECK(expr)
#endif

int main(int argc, char *argv[])
{
    // platform shit starts here ------------------------

    f64 total_time_elapsed = 0;
    f64 dt = 0;

    SDL_CHECK_RES(SDL_Init(SDL_INIT_VIDEO));

    SDL_Window *win = SDL_CreateWindow(
        "television",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600,
        0);

    SDL_CHECK(win);

    SDL_Surface *win_surf = SDL_GetWindowSurface(win);
    SDL_CHECK(win_surf);

    SDL_Event event;
    b8 quit = 0;

    f32 a_ratio = 8 / 6.f;

    //-----------------------------platform shit ends here

    // game boilerplate
    struct YkInput input = {0};

    struct YkGame game = {0};
    yk_innit_game(&game);

    struct render_buffer render_target = {0};
    // SDL_GetWindowSize(win, &render_target.height, &render_target.width);
    render_target.height = 60;
    render_target.width = 80;
    render_target.pixels = malloc(sizeof(u32) * render_target.height * render_target.width);

    SDL_Surface *render_surface = SDL_CreateRGBSurfaceFrom(render_target.pixels, render_target.width, render_target.height, 32, render_target.width * sizeof(u32), 0xFF0000, 0xFF00, 0xFF, 0xFF000000);

    //--------------------
    //ugly fullscreen hack.
    //I dont want to deal with pixel scaling bs.
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
                case SDLK_F1:
                {
                    input.keys[YK_ACTION_HOLD_HANDS] = 1;
                }
                break;
                case SDLK_SPACE:
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
                case SDLK_F1:
                {
                    input.keys[YK_ACTION_HOLD_HANDS] = 0;
                }
                break;

                default:
                    break;
                }
            }
            break;

            case SDL_WINDOWEVENT:
            {
                win_surf = SDL_GetWindowSurface(win);
                SDL_CHECK(win_surf);
            }
            break;
            }
        }

        // game loop start--------
        local_persist f32 fixed_dt;
        fixed_dt += dt;

        if (fixed_dt > 1 / 60.f)
        {
            yk_update_and_render_game(&render_target, &input, &game, fixed_dt);
            fixed_dt = 0;

            SDL_BlitScaled(render_surface, 0, win_surf, 0);

            // Might fail. Leaving it like this until it doesn't
            // It is possible that my window becomes invalid before
            // I recreate it. I could be wrong about this.
            SDL_CHECK_RES(SDL_UpdateWindowSurface(win));
        }

        //-------game loop end

        total_time_elapsed = SDL_GetTicks64() / 1000.f;

        dt = total_time_elapsed - last_time_elapsed;

        // perf stats
#if 1

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