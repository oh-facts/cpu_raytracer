/* date = March 21st 2024 4:28 pm */

#ifndef YK_PLATFORM_H
#define YK_PLATFORM_H
#include <yk_common.h>

struct bitmap
{
    u32* pixels;
    i32 width;
    i32 height;
};

// OS specific initializations
void platform_innit();

#endif //YK_PLATFORM_H
