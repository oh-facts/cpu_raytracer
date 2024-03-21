#include <yk_renderer.h>
/*
    :vomit:
    my alpha blending was a sin so even though I require an alpha channel. I am not going to use it
*/
void draw_rect(struct render_buffer *screen, i32 minx, i32 miny, i32 maxx, i32 maxy, u32 rgba)
{
    if(maxx < 0 || maxy < 0  || minx > screen->width || miny > screen->height)
    {
        return;
    }
    
    minx = (minx < 0) ? 0 : minx;
    miny = (miny < 0) ? 0 : miny;
    maxx = (maxx > screen->width) ? screen->width : maxx;
    maxy = (maxy > screen->height) ? screen->height : maxy;
    
    // ToDo(facts): store pitch inside render_buffer
    u8 *row = (u8 *)screen->pixels + miny * (screen->width * 4) + minx * 4;
    
    for (u32 y = miny; y < maxy; y++)
    {
        u32 *pixel = (u32 *)row;
        
        for (u32 x = minx; x < maxx; x++)
        {
            *pixel++ = rgba;
        }
        row += screen->width * 4;
    }
}

void blit_bitmap_scaled(struct render_buffer* dst, struct render_buffer* src, struct render_rect* dst_rect)
{
    //ToDo(facts): Do proper clipping tests lol
    f32 scale_factor = 0.5f;
    for(u32 y = 0; y < src->height; y++) {
        for(u32 x = 0; x < src->width; x++) {
            u32 pixel = src->pixels[y * src->width + x];
            
            draw_rect(dst,
                      x * scale_factor,
                      y * scale_factor,
                      (x + 1) * scale_factor,
                      (y + 1) * scale_factor,
                      pixel);
        }
    }
}

struct bitmap read_bitmap_file(char* file_data, struct Arena* arena)
{
    u32 * out = 0;
    
    struct BitmapHeader* header = (struct BitmapHeader*)file_data;
    AssertM(header->depth == 32, "your bitmap must use 32 bytes for each pixel");
    
    struct bitmap result;
    result.width = header->width;
    result.height = header->height;
    
    result.pixels = push_array(arena,u32,result.width*result.height);
    
    memcpy(result.pixels, (u8*)file_data + header->pixel_offset,result.width * result.height * sizeof(u32));
    
    // erm, there has to be a better way to flip pixels right?
    u32 temp;
    for (size_t y = 0, yy = result.height; y < yy / 2; y++)
    {
        for (size_t x = 0, xx = result.width; x < xx ; x++)
        {
            temp = result.pixels[y * xx + x];
            result.pixels[y * xx + x] = result.pixels[(yy- 1 - y) * xx + x];
            result.pixels[(yy - 1 - y) * xx + x] = temp;
        }
    }
    
    
    return result;
}

