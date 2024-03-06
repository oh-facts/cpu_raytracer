
#define mem_leak_msvc 0

#include <platform/yk_os.h>
#include <yk_common.h>

#if mem_leak_msvc
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
#endif


int main(int argc, char *argv[])
{

    struct YkWindow win;

    yk_innit_window(&win);

    struct YkClockRaw clock_raw = {0};
    yk_clock_innit(&clock_raw);

    f64 total_time_elapsed = 0;
    f64 dt = 0;

    while (win.win_data.is_running)
    {
        f64 last_time_elapsed = total_time_elapsed;

        // game loop start--------
        if (!win.win_data.is_minimized)
        {
            if (yk_input_is_key_tapped(&win.keys, YK_KEY_ESC))
            {
                // state.shutdown(&state);
                // engine_memory_cleanup(&state.engine_memory);

                printf("We (I) love you");
                // engine_memory_innit(&state.engine_memory);
                // state.start(&state);
            }
        }

        yk_window_update(&win);
        yk_window_poll();

        // testing input
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

    yk_free_window(&win);

#if mem_leak_msvc
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtDumpMemoryLeaks();
#endif
    return 0;
}