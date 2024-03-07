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

    for (u32 i = 0; i < win_surf->w; i++)
    {
        for (u32 j = 0; j < win_surf->h; j++)
        {
            SDL_Rect rect = {0};
            rect.w = 1;
            rect.h = 1;
            rect.x = i;
            rect.y = j;
            SDL_FillRect(win_surf, &rect, SDL_MapRGBA(win_surf->format, j % 256, 0, i % 256, 0xFF));
        }
    }
    SDL_UpdateWindowSurface(win);

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
                if (event.key.keysym.sym == SDLK_q)
                {
                    input.keys[YK_KEY_HOLD_HANDS] = 1;
                }
            }
            break;

            case SDL_KEYUP:
            {
                if (event.key.keysym.sym == SDLK_q)
                {
                    input.keys[YK_KEY_HOLD_HANDS] = 0;
                }
            }
            break;
            }
        }

        // game loop start--------

        handle_hand_holding(&input);

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