#ifndef YK_GAME_H
#define YK_GAME_H

#include <yk_common.h>
#include <yk_arena.h>

union v2i
{
    struct
    {
        i32 x;
        i32 y;
    };
    i32 e[2];
};

typedef union v2i v2i;

enum YK_ACTION
{
    YK_ACTION_HOLD_HANDS = 0,
    YK_ACTION_UP,
    YK_ACTION_DOWN,
    YK_ACTION_LEFT,
    YK_ACTION_RIGHT,
    YK_ACTION_ACCEPT,

    YK_ACTION_COUNT,
};

typedef enum YK_ACTION YK_ACTION;

struct YkInput
{
    b8 keys[YK_ACTION_COUNT];
    b8 keys_old[YK_ACTION_COUNT];
};

#define snake_max_size 10
struct snake
{   
    // 0 is head
    v2i pos[snake_max_size];
    v2i dir;
    i32 size;
};

enum LEVEL
{
    LEVEL_INTRO,
    LEVEL_SNAKE,
};

typedef enum LEVEL LEVEL;

#define MAX_MSG_LEN 128

#define NUM_MSG_1 (5)
#define NUM_MSG_2 (5)

#define NUM_MSG (NUM_MSG_1 + NUM_MSG_2)

const char* messages[NUM_MSG] = {
"Welcome! This is a scripted sequence to help you get started. Press the ENTER button on your remote",
"Warning: Prolonged exposure to the Dear Dear Home System can lead to mild nausea, and in some cases loss of life",
"In most cases, loss of loved ones, and in rare cases, loss of self being. Proceed with caution",
"The DDHS will now look for channels on your local network.",
"Please don't leave",
"Loading ..."
};

struct YkGame
{
    char text[MAX_MSG_LEN];

    //intro stage;
    u8 message_index;

    // stage 1
    struct snake snek;

    LEVEL level;

    // world
    f32 timer;
};

struct render_buffer
{
    u32 *pixels;
    u32 width;
    u32 height;
};

YK_API void yk_innit_game(struct YkGame *game);

YK_API void yk_update_and_render_game(struct render_buffer *screen, struct YkInput *input, struct YkGame *game, f32 delta);

#endif