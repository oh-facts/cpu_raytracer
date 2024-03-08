#include <yk_game.h>

#define speed 5

YK_API void handle_hand_holding(struct YkInput *input, struct YkGame *game)
{
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
}

YK_API void render(struct render_buffer *screen, struct YkGame *game)
{
    u32 width = screen->width;
    u32 height = screen->height;
    u32 pos_x = game->pos_x;
    u32 pos_y = game->pos_y;
    u32 *pixels = screen->pixels;

    for (u32 j = 0; j < height; j++)
    {
        for (u32 i = 0; i < width; i++)
        {
            u32 posX = i - pos_x;
            u32 posY = j - pos_y;

            (pixels)[i + width * j] = (0xFF << 24) | ((posX % 256) << 16) | ((0) << 8) | (posY % 256);
        }
    }
}