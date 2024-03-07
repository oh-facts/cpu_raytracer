#ifndef YK_GAME_H
#define YK_GAME_H

#include <yk_common.h>
#include <yk_arena.h>

enum YK_KEY
{
    YK_KEY_HOLD_HANDS = 0,
    YK_KEY_COUNT
};

typedef enum YK_KEY YK_KEY;

struct YkInput
{
    b8 keys[YK_KEY_COUNT];
};

YK_API void handle_hand_holding(struct YkInput* input);

#endif