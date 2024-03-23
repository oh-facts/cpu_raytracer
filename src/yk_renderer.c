#include <yk_renderer.h>
#include <stb_truetype.h>

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
            if(((rgba >> 24) & 0xFF) < 255)
            {
                continue;
            }
            
            *pixel++ = rgba;
        }
        row += screen->width * 4;
    }
}

void blit_bitmap(struct bitmap* dst, struct bitmap* src, struct render_rect* dst_rect)
{
    for(u32 y = 0; y < src->height; y++)
    {
        for(u32 x = 0; x < src->width; x++)
        {
            u32 pixel = src->pixels[y * src->width + x];
            
            draw_rect(dst,
                      x + dst_rect->x,
                      y + dst_rect->y,
                      x + dst_rect->x+ 1,
                      y+ dst_rect->y+ 1,
                      pixel);
        }
    }
}


void blit_bitmap_scaled(struct render_buffer* dst, struct render_buffer* src, struct render_rect* dst_rect)
{
    f32 scaleX = (f32)dst_rect->w / (f32)src->width;
    f32 scaleY = (f32)dst_rect->h / (f32)src->height;
    
    for(u32 y = 0; y < src->height; y++)
    {
        for(u32 x = 0; x < src->width; x++)
        {
            u32 pixel = src->pixels[y * src->width + x];
            
            f32 destX = dst_rect->x + (x * scaleX);
            f32 destY = dst_rect->y + (y * scaleY);
            
            draw_rect(dst,
                      destX,
                      destY,
                      destX + scaleX,
                      destY + scaleY,
                      pixel);
        }
    }
}


//https://en.wikipedia.org/wiki/BMP_file_format
#pragma pack(push, 1)
struct BitmapHeader
{
    //file header
    u16 file_type;
    u32 file_size;
    u16 _reserved;
    u16 _reserved2;
    u32 pixel_offset;
    
    //DIB header
    u32 header_size;
    i32 width;
    i32 height;
    u16 color_panes;
    u16 depth;// stored in bits
};
#pragma pack(pop)

struct bitmap make_bmp_from_file(char* file_data, struct Arena* arena)
{
    u32 * out = 0;
    
    struct BitmapHeader* header = (struct BitmapHeader*)file_data;
    AssertM(header->depth == 32, "your bitmap must use 32 bytes for each pixel");
    
    struct bitmap result = {0};
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

struct bitmap make_bmp_font(char* file_data, char codepoint,  struct Arena* arena)
{
    
    struct bitmap out = {0};
    stbtt_fontinfo font;
    stbtt_InitFont(&font, (u8*)file_data, stbtt_GetFontOffsetForIndex((u8*)file_data,0));
    
    i32 w,h,xoff,yoff;
    u8* bmp = stbtt_GetCodepointBitmap(&font, 0, stbtt_ScaleForPixelHeight(&font, 550), codepoint ,&w,&h, &xoff, &yoff);
    
    out.width = w;
    out.height = h;
    out.pixels = push_array(arena,u32,w * h);
    
    u8* src = bmp;
    u8* dest_row = (u8*)out.pixels;
    
    for(u32 y = 0; y < h; y ++)
    {
        u32* dest = (u32*)dest_row;
        for(u32 x = 0; x < w; x ++)
        {
            u8 alpha = *src++;
            *dest++ = ((alpha <<24) |
                       (alpha <<16) |
                       (alpha << 8) |
                       (alpha ));
        }
        dest_row += 4 * out.width;
    }
    
    stbtt_FreeBitmap(bmp, 0);
    return out;
}