#ifndef YK_GAME_H
#define YK_GAME_H

#include <yk_common.h>
#include <yk_arena.h>
#include <yk_math.h>

enum YK_ACTION
{
    YK_ACTION_HOLD_HANDS = 0,
    YK_ACTION_UP,
    YK_ACTION_DOWN,
    YK_ACTION_LEFT,
    YK_ACTION_RIGHT,
    YK_ACTION_COUNT
};

typedef enum YK_ACTION YK_ACTION;

struct YkInput
{
    b8 keys[YK_ACTION_COUNT];
};

struct YkGame
{
    u32 pos_x;
    u32 pos_y;
};

YK_API void handle_hand_holding(struct YkInput* input, struct YkGame* game);

#endif