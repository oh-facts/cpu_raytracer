#include <yk_game.h>

#define speed 15

static u32 g_seed = 42;

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

YK_API void yk_update_and_render_game(struct render_buffer *screen, struct YkInput *input, struct YkGame *game)
{
    // update game
    if (input->keys[YK_ACTION_UP] == 1)
    {
        game->pos_y += speed;
    }
    if (input->keys[YK_ACTION_DOWN] == 1)
    {
        game->pos_y -= speed;
    }
    if (input->keys[YK_ACTION_LEFT] == 1)
    {
        game->pos_x += speed;
    }
    if (input->keys[YK_ACTION_RIGHT] == 1)
    {
        game->pos_x -= speed;
    }

    // render game
    u32 width = screen->width;
    u32 height = screen->height;
    u32 pos_x = game->pos_x;
    u32 pos_y = game->pos_y;
    u32 *pixels = screen->pixels;

    for (u32 i = 0; i < height; i++)
    {
        u32 posY = i - pos_y;

        for (u32 j = 0; j < width; j++)
        {
#if 1
            pixels[width * i + j] = (0xFF << 24) | (test_rand() << 16) | (test_rand() << 8) | test_rand();
#elif 0

            u32 posX = j - pos_x;
            pixels[width * i + j] = (0xFF << 24) | ((posX % 256) << 16) | ((0) << 8) | (posY % 256);

#endif
        }
    }
}