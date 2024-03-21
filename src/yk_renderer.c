#include <renderer.h>

// I don't want to use sdl2 to render
// this is temporary till I can render text myself
#inclide <platform/yk_sdl2_include.h>

void blit_text(const char* text, struct render_buffer* buffer)
{
    TTF_Font* font = TTF_OpenFont("../res/Delius-Regular.ttf",48);
    
    if(!font)
    {
        printf("%s",SDL_GetError());
    }
    
    SDL_Surface* text = TTF_RenderText_Solid(font, text, (SDL_Color){255,0,0});
    SDL_BlitSurface(text, 0, buffer, 0);
}