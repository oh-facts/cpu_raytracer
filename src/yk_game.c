#include <yk_game.h>

YK_API void handle_hand_holding(struct YkInput* input)
{
    if(input->keys[YK_KEY_HOLD_HANDS] == 1)
    {
        printf(":flushed:");
    }
}