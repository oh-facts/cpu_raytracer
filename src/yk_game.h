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
    //player data
    u32 pos_x;
    u32 pos_y;
    u32 width;
    u32 height;

    //world
    f32 timer;
};

struct render_buffer
{
    u32* pixels;
    u32 width;
    u32 height;
};

YK_API void yk_innit_game(struct YkGame* game);

YK_API void yk_update_and_render_game(struct render_buffer* screen, struct YkInput *input, struct YkGame * game, f32 delta );

#endif