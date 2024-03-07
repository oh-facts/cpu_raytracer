#include <yk_platform.h>
#include <Windows.h>

void yk_clock_innit(struct YkClockRaw *self)
{
    LARGE_INTEGER start_counter = {0};
    QueryPerformanceCounter(&start_counter);
    self->start = start_counter.QuadPart;

    LARGE_INTEGER perf_freq = {0};
    QueryPerformanceFrequency(&perf_freq);
    self->freq = perf_freq.QuadPart;
}

f64 yk_get_time_since(struct YkClockRaw *self)
{
    LARGE_INTEGER end_counter = {0};
    QueryPerformanceCounter(&end_counter);

    i64 counter_elapsed = end_counter.QuadPart - self->start;
    return (1.f * counter_elapsed) / self->freq;
}