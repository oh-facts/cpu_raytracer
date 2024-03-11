#include <yk_game.h>

#define speed 1

internal u32 g_seed = 42;

YK_API void yk_innit_game(struct YkGame *game)
{
    game->height = 4;
    game->width = 4;
}

void draw_rect(struct render_buffer *screen, u32 minx, u32 miny, u32 maxx, u32 maxy, u32 rgba)
{
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

// lcg
u32 lcg_rand()
{
    g_seed = 214013 * g_seed + 2531011;
    return (g_seed >> 16);
}

/*
    Testing different rand functions
    rand() sucks so so hard balls (speedwise)
    its unbelievable
*/
u32 test_rand()
{
#if 1
    return lcg_rand() % 0xFF;
#endif

#if 0
    return rand() % 0xFF;
#endif
}

YK_API void yk_update_and_render_game(struct render_buffer *screen, struct YkInput *input, struct YkGame *game, f32 delta)
{
    game->timer += delta;

    // update game
    if (input->keys[YK_ACTION_UP] == 1)
    {
        if (game->pos_y > 0)
            game->pos_y -= speed;
    }
    if (input->keys[YK_ACTION_DOWN] == 1)
    {
        if (game->pos_y < screen->height - game->height)
            game->pos_y += speed;
    }
    if (input->keys[YK_ACTION_LEFT] == 1)
    {
        if (game->pos_x > 0)
            game->pos_x -= speed;
    }
    if (input->keys[YK_ACTION_RIGHT] == 1)
    {
        if (game->pos_x < screen->width - game->width)
            game->pos_x += speed;
    }

    if (game->timer > 1 / 12.f)
    {
        game->timer = 0;
        u32 width = screen->width;
        u32 height = screen->height;
        u32 *pixels = screen->pixels;

        for (u32 i = 0; i < height; i++)
        {

            for (u32 j = 0; j < width; j++)
            {
                pixels[width * i + j] = (0xFF << 24) | ((test_rand()) << 16) | (test_rand() << 8) | test_rand();
            }
        }

        //need alpha blending for this
       // draw_rect(screen, 0, 0, screen->width / 2, screen->height / 2, 0x000000FF);

        // player
        draw_rect(screen, game->pos_x, game->pos_y, game->pos_x + game->width, game->pos_y + game->height, 0xFF000000);
        
        

    }

    // draw_rect(screen, 4, 4, 10, 10, 0xFF000000);

    // ToDo(facts): Just accept rect. I dont want to do this disco with variables
    
    // printf("[%d %d]\n", game->pos_x, game->pos_y);

#if 0
    if (input->keys[YK_ACTION_HOLD_HANDS] == 1)
    {
        for (u32 i = 0; i < height; i++)
        {

            for (u32 j = 0; j < width; j++)
            {
                printf("%d ", screen->pixels[width * i + j]);
            }
            printf("\n");
        }
    }
#endif
}
