/* date = March 20th 2024 11:18 pm */

#ifndef YK_RENDERER_H
#define YK_RENDERER_H

#include <yk_common.h>
#include <yk_arena.h>

struct render_rect
{
    i32 x,y,w,h;
};

struct bitmap
{
    void* pixels;
    i32 width;
    i32 height;
    i32 depth;
};

// Note(facts): This is stored upside down. So I flip it (along x axis)
YK_API struct bitmap make_bmp_from_file(char* file_data, struct Arena* arena);

YK_API void blit_bitmap(struct bitmap* dst, struct bitmap* src, struct render_rect* dst_rect);

YK_API void blit_bitmap_scaled(struct bitmap* dst, struct bitmap* src, struct render_rect* dst_rect);

YK_API void draw_rect(struct bitmap *dst, i32 minx, i32 miny, i32 maxx, i32 maxy, u32 rgba);

YK_API struct bitmap make_bmp_font(char* file_data, char codepoint,  struct Arena* arena);

#endif //YK_RENDERER_H