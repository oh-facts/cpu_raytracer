#include <yk_game.h>

#define speed 5

YK_API void handle_hand_holding(struct YkInput* input, struct YkGame* game)
{
    if(input->keys[YK_ACTION_UP] == 1)
    {
        game->pos_y += speed;
    }
    if(input->keys[YK_ACTION_DOWN] == 1)
    {
        game->pos_y -= speed;
    }
    if(input->keys[YK_ACTION_LEFT] == 1)
    {
        game->pos_x += speed;
    }
    if(input->keys[YK_ACTION_RIGHT] == 1)
    {
        game->pos_x -= speed;
    }

}