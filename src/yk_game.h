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

enum SNAKE_WAVE
{
    /*
        starter apples
    */
    SNAKE_WAVE_1,
    SNAKE_WAVE_BUFFER,
    SNAKE_WAVE_2,
    SNAKE_WAVE_NUM,
};

typedef enum SNAKE_WAVE SNAKE_WAVE;

enum LEVEL
{
    LEVEL_INTRO,
    LEVEL_SNAKE,
};

typedef enum LEVEL LEVEL;

#define MAX_MSG_LEN 128


enum YKMSG
{
    MSG_INTRO_0,
    MSG_INTRO_1,
    MSG_INTRO_2,
    MSG_INTRO_3,
    MSG_INTRO_4,
    MSG_INTRO_5,

    MSG_SNAKE_0,
    MSG_SNAKE_1, 
    MSG_SNAKE_2, 
};

typedef enum YKMSG YKMSG;

#define MSG_L1S1     (MSG_INTRO_0)
#define MSG_L2S1     (MSG_SNAKE_0)
#define MSG_L2S2     (7)


#define NUM_MSG (NUM_MSG_1 + NUM_MSG_2)

const char* messages[25] = {
"Welcome! This is a scripted sequence to help you get started. Press the ENTER button on your remote",
"Warning: Prolonged exposure to the Dear Dear Home System can lead to mild nausea, and in some cases loss of life",
"In most cases, loss of loved ones, and in rare cases, loss of self being. Proceed with caution",
"The DDHS will now look for channels on your local network. This will take a while",
"Please don't leave",
"Here is a song I wrote (for occasions like these)",

"Trying to connect",
"Lost connection",
"Connecting Very Fast!",
"Loading at record speed!",
"You can't eat that pixel. It is dead.",
"I don't know how to find channels",
"But I can play music.",
"You can't eat that pixel. It is dead. I am dying",
"Please stop eating pixels"
// this goes on till everything is black because pixels keep generating.
// And then you explore the puzzle
// But you will grow small and die if you don't eat pixels so you must eat pixels
// I will communicate with flashing red
// No No No
// you eat pixels. you get bigger in size. then they disappear. then you go back to horizontal
// way and then it says, "loading faster"
// more pixels appear
// "loading very fast"
// more pixels
// can't eat anymore
// i'm dying
// can't cross self perhaps either?
};

struct YkGame
{
    // stage 0;
    v2i loading_bar;

    // stage 1
    struct snake snek;
    u32 eaten;
    SNAKE_WAVE wave;
    u32 msg_index;
    u32 msg_last;

    LEVEL level;

    // world
    f32 timer;
    char bgm[32];
    char alert_sound[32];

    //platform
    void * _win;
    void (*platform_play_audio)(const char* path);
    void (*platform_set_title)(void * win, const char* title);
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