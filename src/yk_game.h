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

// ToDo(facts): I want to add this file to "sand box"
// for that I need to make it so I can avoid having
// game data inside this header. Game data that will
// be hard to remove is my actions. I can make it so
// that these are generic key names and inside my game.c
// I can map the enum to my own actions. This is a pretty
// good idea. I should do this.
enum YK_ACTION
{
    YK_ACTION_HOLD_HANDS = 0,
    YK_ACTION_UP,
    YK_ACTION_DOWN,
    YK_ACTION_LEFT,
    YK_ACTION_RIGHT,
    YK_ACTION_ACCEPT,
    YK_ACTION_SAVE,
    YK_ACTION_RESTORE,
    
    YK_ACTION_COUNT,
};

typedef enum YK_ACTION YK_ACTION;

struct YkInput
{
    b8 keys[YK_ACTION_COUNT];
    b8 keys_old[YK_ACTION_COUNT];
};


//ToDo(facts): Often I overflow the stack.
//I could use a ll. Or have an assert that
//adds up all apples to guarantee that the
// sum of apples + initial lenght is always
// less than max size.
// or the most sane thing, when eating apples,
// put an upper limit on size. And print to
// the console that this is technically undesired?
// I only get the overflows when I am trying stuff
// I don't really intend to have the snake game
// long enough for this to matter.
#define SNAKE_MAX_SIZE 15
struct snake
{   
    // 0 is head
    v2i pos[SNAKE_MAX_SIZE];
    v2i dir;
    i32 size;
};

/*
    ToDo(facts): Use better naming for the enums
*/

enum SNAKE_WAVE
{
    
    //starter apples. After eating all the apples you go the next stage.
    // In this stage you align yourself with the bar. And when you are alligned //you go to the next stage
    SNAKE_WAVE_START,
    
    // After being aligned for a few seconds, you go to the next stage
    SNAKE_WAVE_ALIGN_WAIT,
    
    // After eating all dead pixels, you complete this level
    SNAKE_WAVE_DEAD_PIXELS,
    
    SNAKE_WAVE_NUM,
};

typedef enum SNAKE_WAVE SNAKE_WAVE;

enum LEVEL
{
    // The messages in the beginning before the snake game starts
    LEVEL_INTRO,
    
    // snake stage
    LEVEL_SNAKE,
    
    // After eating all dead pixels, you start the outro stage
    LEVEL_OUTRO,
};

typedef enum LEVEL LEVEL;

#define MAX_MSG_LEN 128

/*
    ToDo(facts): use better names.
    These have been very unreadable.
    I am considering using defines with
    strings next time.

Note(facts): I was able to make modifications
with very little effort. I think this way is fine.
Only that I should use better names. MSG_INTRO_# is bad.
Use actual names after MSG_INTRO.

Also, maybe I can have something called sequences that have
sequences enums. And another that just has messages. And these
are more general messages.
*/

enum YKMSG
{
    MSG_INTRO_0,
    MSG_INTRO_1,
    MSG_INTRO_2,
    MSG_INTRO_3,
    MSG_INTRO_4,
    MSG_INTRO_5,
    MSG_INTRO_6,
    
    //snake start, snake align
    MSG_SNAKE_TRYING_TO_CONN,
    MSG_SNAKE_LOST_CONN, 
    
    //snake align wait
    MSG_SNAKE_ALIGN,
    
    MSG_DEAD_PIXELS_1,
    MSG_DEAD_PIXELS_2,
    MSG_DEAD_PIXELS_3,
    
    MSG_OUTRO_1,
    MSG_OUTRO_2,
    MSG_OUTRO_3,
    MSG_OUTRO_4,
    MSG_OUTRO_5,
    MSG_OUTRO_6,
    MSG_OUTRO_7,
    MSG_OUTRO_8,
    
    MSG_BYE,
    
    MSG_NUM
};

typedef enum YKMSG YKMSG;

// I think i want to use a #define with strings
// and map them to enums.
// Future me: What do I mean by that? How would I map #defines to enums
const char* messages[25] = {
    
    //intro
    "Welcome! This is a scripted sequence to help you with setup. Press the ENTER button on your remote",
    "Warning: Prolonged exposure to the Dear Dear Home System can lead to mild nausea, and in some cases loss of life",
    "In most cases, loss of loved ones, and in rare cases, loss of self being. Proceed with caution",
    "Remember. You can *always* press S to save and R to return, incase you make a mistake in the setup.",
    "The DDHS will now look for channels on your local network. This will take a while",
    "Please don't leave",
    "Here is a song I wrote (for occasions like these)",
    
    //snake start , snake align
    "Trying to connect (This can take upto 3 minutes)",
    "Lost connection :(",
    
    //snake align wait
    "Established connection! Initializing ...",
    
    //dead pixels
    "Those pixels are dead. I am dying.",
    "If you eat them. I will get the pixels back.",
    "I don't wan't to die. Can I have the pixels back?",
    
    // snake outro
    "I am tired. I can't find channels at this rate",
    "I don't even know how to find channels",
    "But I can play music",
    
    //programmer's note: This was going to be, "Let me play something else"
    // but Its too much work to do this right and I am too tired to read
    // the example. I want to sleep
    "I would play something else. But this is the only song I know",
    "I am sorry for wasting your time",
    "Maybe I will do better next time",
    "Also",
    "dear dear loves you",
    "Bye"
        
};


#define MAX_APPLES (10)

#define SNAKE_LEVEL_START_APPLE_NUM      (6)
#define SNAKE_LEVEL_DEAD_PIXEL_APPLE_NUM (4)

#include <yk_renderer.h>
#include <platform/yk_platform.h>

// ToDo(facts): Instead of having next wave initialization data inside the  if inside waves to go to the next wave,
// make a next_wave function that is similar to next level load.
// or heck just have a nested switch inside load level and change waves.
// I like that more actually
struct YkGame
{
    //render context
    u32 opa;
    struct bitmap main;
    struct bitmap ui;
    
    // stage 0;
    v2i loading_bar;
    u32 loading_bar_color;
    
    // stage 1
    struct snake snek;
    u32 eaten;
    SNAKE_WAVE wave;
    u32 msg_index;
    u32 msg_last;
    v2i apples[MAX_APPLES];
    u32 num_apples;
    
    //wavestart
    f32 align_timer;
    b8 align_msg_flag;
    
    //align wait
    f32 align_wait_timer;
    b8 align_wait_flag;
    
    // outro
    
    LEVEL level;
    
    // world
    // I have been misusing this value. Been modifying it willy nilly.
    // Rework it. Or call it an apropriate name.
    f32 timer;
    
    u32 bgm;
    u32 alert_sound;
    
    i32 width;
    i32 height;
    
    struct YkGame* saved;
    YKMSG last_msg;
    
    struct Arena arena;
    struct Arena scratch;
    
    struct bitmap rabbit;
    struct bitmap welcome;
    struct bitmap words[27];
    struct bitmap dear;
    //platform
    void * _win;
    u32 (*platform_innit_audio)(const char* path);
    void (*platform_play_audio)(u32 audio_id);
    void (*platform_stop_audio)(u32 audio_id);
    void (*platform_set_title)(void * win, const char* title);
    char* (*platform_read_file)(const char* filename, struct Arena* arena);
    
};


typedef void (*yk_innit_game_func)(struct YkGame *game, struct offscreen_buffer *screen);
typedef void (*yk_update_and_render_game_func)(struct offscreen_buffer *screen, struct YkInput *input, struct YkGame *game, f32 delta);

#endif