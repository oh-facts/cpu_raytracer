#ifndef YK_PLATFORM_H
#define YK_PLATFORM_H

#include <yk_common.h>
#include <yk_arena.h>

//ToDo(facts)
//different build profiles
//for different OSs

struct YkClockRaw
{
    i64 freq;
    i64 start;
};

YK_API void yk_clock_innit(struct YkClockRaw *self);

YK_API f64 yk_get_time_since(struct YkClockRaw *self);

#endif