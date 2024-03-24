/* date = March 21st 2024 4:28 pm */

#ifndef YK_PLATFORM_H
#define YK_PLATFORM_H
#include <yk_common.h>


/*
ToDo(Facts):
I am abandoning this project indefinitely. Here is my immediate to do for now, and since this is my most well made renderer so far, I will treat this with some respect.

1. Remove some of the game code inside the platform layer. Why is bitmap here? That is renderer stuff. render buffer should be here.

2. Optmize your renderer. Make a command buffer and draw to that, like you did in vulkan (This will eventually receive a vulkan renderer for 3d). I am doing it in "immediate" mode currently. (Technically I am doing everything on cpu so I can't imagine doing in a cmd buffer will this any faster)

3. My blitting functions work well, except my rects aren't dst rects. They are very clearly src rects. Basically, work on the API

This is all for now. Engine + platform is still only ~600 loc so its very manageable and I've been deleting dead code.

I have other committments and I will work on that. Television game is not dead.
*/

bitmap
{
    u32* pixels;
    i32 width;
    i32 height;
};

// OS specific initializations
void platform_innit();

#endif //YK_PLATFORM_H
