#include <yk_game.h>

internal u32 g_seed = 42;
internal u32 test_rand();
internal u32 lcg_rand();

#define SNAKE_SPEED 5
#include <string.h>

#define WHITE (0xFFFFFFFF)
#define BLACK (0xFF000000)

void load_level(struct YkGame* game);

#define RENDER_STATIC(screen, mode) \
_render_static_##mode(screen)

internal void _render_static_STATIC_MODE_BASIC(struct render_buffer * screen);
internal void _render_static_STATIC_MODE_HALF(struct render_buffer * screen);
internal void _render_static_STATIC_MODE_MIXED(struct render_buffer * screen);
internal void _render_static_STATIC_MODE_MIXED_2(struct render_buffer * screen);
internal void _render_static_STATIC_MODE_BLACK(struct render_buffer * screen);

internal void draw_rect(struct render_buffer *screen, u32 minx, u32 miny, u32 maxx, u32 maxy, u32 rgba);

#define V2I_FMT "%d %d"
#define V2I_(s) (s).x, (s.y)

#define MAX_APPLES 10
internal v2i apples[MAX_APPLES] = {};
internal const u32 apple_num_index[SNAKE_WAVE_NUM] = {5,3,4,5};

internal void reset_apples()
{
    for(u32 i = 0; i < MAX_APPLES; i ++)
    {
        apples[i] = (v2i){-100, -100};
    }
}

internal void randomise_apples(u32 width, u32 height, u32 num)
{
    for(u32 i = 0; i < num; i ++)
    {
        apples[i] = (v2i){lcg_rand() % width,lcg_rand() % height};
    }
}

internal const v2i loading_bar = (v2i){4, 50};

internal void draw_bar(struct render_buffer* screen, struct YkGame *game)
{
    v2i _lb = game->loading_bar;
    draw_rect(screen, _lb.x ,_lb.y, _lb.x + 4, _lb.y + 4, 0xFFFFFFFF);
}

internal void draw_apples(struct render_buffer * screen, u32 color)
{
    for(u32 i = 0; i < MAX_APPLES; i ++)
    {
        draw_rect(screen, apples[i].x, apples[i].y, apples[i].x + 2, apples[i].y + 2, color);
    }
}

internal void snake_mv(struct YkGame * game, struct YkInput *input);

internal void snake_apple_collision(struct YkGame* game, const u32 apple_num, b8 grow)
{
    struct snake *snek = &game->snek;
    // check and eat apple. using broadphase SAT AABB. Might optmize with quadtrees
    for(u32 i = 0; i < apple_num; i ++)
    {
        if((abs(snek->pos[0].x - apples[i].x) < 4) && ((abs(snek->pos[0].y - apples[i].y) < 4)))
        {
            
            apples[i].x = -100; //lcg_rand() % screen->width;
            apples[i].y = -100; //lcg_rand() % screen->height;
            
            if(grow)
            {
                snek->pos[snek->size] = snek->pos[snek->size - 1];
                snek->size ++;
            }
            else
            {
                snek->size --;
            }
            
            game->eaten ++;
            //printf("Gulp %d\n",i);
        }
    }
}

//Deeps clone game data. Exists so I can save.
internal void game_data_clone(struct YkGame* dst, struct YkGame* src)
{
    dst->loading_bar = src->loading_bar;
    dst->snek        = src->snek;
    dst->eaten       = src->eaten;
    dst->msg_index   = src->msg_index;
    dst->msg_last    = src->msg_last;
    dst->level       = src->level;
    dst->timer       = src->timer;
    
    strcpy(dst->bgm,src->bgm);
    strcpy(dst->alert_sound,src->alert_sound);
    
    dst->width       = src->width;
    dst->height      = src->height;
    dst->saved       = src->saved;
    dst->_win        = src->_win;
    
    dst->platform_play_audio = src->platform_play_audio;
    dst->platform_set_title = src->platform_set_title;
}

internal void game_data_save(struct YkGame* game)
{
    printl("saved game");
    game_data_clone(game->saved,game);
}

internal void game_data_restore(struct YkGame* game)
{
    printl("restore game");
    game_data_clone(game,game->saved);
}

internal b8 snake_loading_bar_collision(struct snake* snek)
{
    if((abs(snek->pos[0].y - loading_bar.y) <= 6) && snek->dir.x == 1)
    {
        return 1;
    }
    return 0;
}

// 0 if snake doesn't hit.
// index if it does hit
internal u32 snake_self_collision(struct snake* snek)
{
    
    //It seems it is impossible for a snake to eat body index < 3?
    for(u32 i = 3; i < snek->size; i ++)
    {
        //printf("%d: " V2I_FMT  "\n",i,V2I_(snek->pos[i]));
        if((snek->pos[i].x == snek->pos[0].x ) &&
           (snek->pos[i].y == snek->pos[0].y ))
        {
            return i;
        }
    }
    
    return 0;
}

internal void snake_eat_self(struct snake* snek)
{
    u32 hit_i = snake_self_collision(snek);
    
    if(hit_i)
    {
        snek->size = hit_i;
        
        //printf("hit at %d\n",hit_i);
    }
}

internal void send_msg(struct YkGame* game, YKMSG msg)
{
    
    game->platform_set_title(game->_win, messages[msg]);
    game->platform_play_audio(game->alert_sound);
    //printf("w");
}

YK_API void yk_innit_game(struct YkGame *game)
{
    //ToDo(facts): remove allocations
    game->saved = malloc(sizeof(struct YkGame));
    
    strcpy(game->bgm,"../res/song0.wav");
    strcpy(game->alert_sound, "../res/GameAlert.wav");
    
    game->level = 0;
    game->width = 80;
    game->height = 60;
    //game->wave = SNAKE_WAVE_3;
    load_level(game);
}

void load_level(struct YkGame* game)
{
    game->timer = 0;
    switch (game->level)
    {
        case LEVEL_INTRO:
        {
            game->msg_index = MSG_INTRO_0;
            game->platform_set_title(game->_win, messages[game->msg_index++]);
            game->loading_bar = loading_bar;
        }break;
        case LEVEL_SNAKE:
        {
            reset_apples();
            randomise_apples(80, 60, apple_num_index[game->wave]);
            struct snake *snek = &game->snek;
            snek->size = 10;
            snek->dir = (v2i){1, 0};
            snek->pos[0] = loading_bar;
            
            game->platform_set_title(game->_win, messages[MSG_SNAKE_0]);
            
        }break;
        
        default:
        {
            
        }break;
    }
    
    
}


enum STATIC_MODE
{
    STATIC_MODE_BASIC,
    STATIC_MODE_MIXED,
    STATIC_MODE_HALF,
};

typedef enum STATIC_MODE STATIC_MODE;

/*
    :vomit:
    my alpha blending was a sin so even though I require an alpha channel. I am not going to use it
*/
void draw_rect(struct render_buffer *screen, u32 minx, u32 miny, u32 maxx, u32 maxy, u32 rgba)
{
    minx = (minx < 0) ? 0 : minx;
    miny = (miny < 0) ? 0 : miny;
    maxx = (maxx > screen->width) ? screen->width : maxx;
    maxy = (maxy > screen->height) ? screen->height : maxy;
    
    // ToDo(facts): store pitch inside render_buffer
    u8 *row = (u8 *)screen->pixels + miny * (screen->width * 4) + minx * 4;
    
    for (u32 y = miny; y < maxy; y++)
    {
        u32 *pixel = (u32 *)row;
        
        for (u32 x = minx; x < maxx; x++)
        {
            *pixel++ = rgba;
        }
        row += screen->width * 4;
    }
}

b8 yk_input_is_key_tapped(struct YkInput *state, u32 key)
{
    return state->keys[key] && !state->keys_old[key];
}

b8 yk_input_is_key_held(struct YkInput *state, u32 key)
{
    return state->keys[key];
}

YK_API void yk_update_and_render_game(struct render_buffer *screen, struct YkInput *input, struct YkGame *game, f32 delta)
{
    game->timer += delta;
    switch (game->level)
    {
        case LEVEL_INTRO:
        {
            game->loading_bar.x = game->loading_bar.x < screen->width ? game->loading_bar.x + 1 : 0;
            if(yk_input_is_key_tapped(input,YK_ACTION_ACCEPT))
            {
                if(game->msg_index == MSG_INTRO_5)
                {
                    game->platform_play_audio(game->bgm);
                }
                
                if(game->msg_index == MSG_L2S1)
                {
                    game->level ++;
                    load_level(game);
                    break;
                }
                
                send_msg(game, game->msg_index++);
                
            }
            if (game->timer > 1 / 12.f)
            {
                game->timer = 0;
                RENDER_STATIC(screen, STATIC_MODE_BASIC);
                
                //loading bar
                if(game->msg_index > MSG_INTRO_3)
                {
                    draw_bar(screen,game);                    
                }
                
            }
            
        }break;
        case LEVEL_SNAKE:
        {
            struct snake *snek = &game->snek;
            
            
            if (yk_input_is_key_tapped(input, YK_ACTION_UP))
            {
                snek->dir = (v2i){0, -1};
            }
            if (yk_input_is_key_tapped(input, YK_ACTION_DOWN))
            {
                snek->dir = (v2i){0, 1};
            }
            if (yk_input_is_key_tapped(input, YK_ACTION_LEFT))
            {
                snek->dir = (v2i){-1, 0};
            }
            if (yk_input_is_key_tapped(input, YK_ACTION_RIGHT))
            {
                snek->dir = (v2i){1, 0};
            }
            
            if (yk_input_is_key_tapped(input, YK_ACTION_SAVE))
            {
                game_data_save(game);
            }
            
            if (yk_input_is_key_tapped(input, YK_ACTION_RESTORE))
            {
                game_data_restore(game);
            }
            
            // sim world
            if (game->timer > 1 / 12.f)
            {
                game->timer = 0;
                
                switch (game->wave)
                {
                    
                    case SNAKE_WAVE_1:
                    {
                        snake_eat_self(snek);
                        
                        //draw my life
                        RENDER_STATIC(screen, STATIC_MODE_MIXED);
                        draw_apples(screen, WHITE);
                        snake_mv(game,input);
                        // check and eat apple. using broadphase SAT AABB. Might optmize with quadtrees
                        snake_apple_collision(game, apple_num_index[game->wave],1);
                        
                        if(game->eaten == apple_num_index[game->wave])
                        {
                            game->wave ++;                            
                        }
                        
                        internal u8 flag;
                        
                        if(snake_loading_bar_collision(snek))
                        {
                            if(flag == 0)
                            {
                                send_msg(game,MSG_SNAKE_0);
                                flag = 1;
                            }
                        }
                        else
                        {
                            if(flag == 1)
                            {
                                send_msg(game,MSG_SNAKE_1);
                                flag = 0;
                            }
                            
                        }
                        
                    }break;
                    case SNAKE_WAVE_BUFFER:
                    {
                        RENDER_STATIC(screen, STATIC_MODE_MIXED);
                        internal f32 eep_timer;
                        snake_mv(game,input);
                        internal b8 flag;
                        if(snake_loading_bar_collision(snek))
                        {
                            eep_timer += delta * 6;
                            if(eep_timer > 3.f)
                            {
                                send_msg(game,MSG_L2S2);
                                snek->pos[0] = (v2i){-1,-1};
                                game->wave ++;
                                
                            }
                            if(flag == 0)
                            {
                                send_msg(game,MSG_SNAKE_0);
                                flag = 1;
                            }
                        }
                        else
                        {
                            if(flag == 1)
                            {
                                send_msg(game,MSG_SNAKE_1);
                                flag = 0;
                            }
                            eep_timer = 0;
                            
                        }
                        
                        
                        //printf("%d\n",game->wave);
                        
                        
                        
                    }break;
                    case SNAKE_WAVE_2:
                    {
                        RENDER_STATIC(screen, STATIC_MODE_MIXED);
                        internal f32 counter;
                        counter += delta * 4;
                        
                        game->loading_bar.x = game->loading_bar.x < screen->width ? game->loading_bar.x + SNAKE_SPEED : 0;
                        draw_bar(screen,game);
                        internal b8 flag;
                        if(counter > 3 && !flag)
                        {
                            flag = 1;
                            //send_msg(game, MSG_SNAKE_3);
                            game->wave ++;
                            game->eaten = 0;
                            reset_apples();
                            randomise_apples(80,30,apple_num_index[game->wave]);
                            snek->pos[0] = loading_bar;
                            break;
                        }
                        
                        //snake_apple_collision(game,apple_num_index[game->wave]);
                        
                        
                    }break;
                    case SNAKE_WAVE_3:
                    {
                        RENDER_STATIC(screen, STATIC_MODE_MIXED_2);
                        draw_apples(screen, WHITE);
                        snake_mv(game,input);
                        snake_apple_collision(game,apple_num_index[game->wave],0);
                        
                        internal b8 flag;
                        if(game->eaten == apple_num_index[game->wave] && !flag)
                        {
                            send_msg(game,MSG_L2S3);
                            game->msg_index = MSG_L2S4;
                            flag = 1;
                            game->level ++;
                        }
                        
                    }break;
                    
                    default:
                    {
                        printf("ooga booga why is control here");
                        
                    }break;
                    
                    
                }
                
                
                // draw snake
                for (u32 i = 0; i < snek->size; i++)
                {
                    draw_rect(screen, snek->pos[i].x, snek->pos[i].y, snek->pos[i].x + 4, snek->pos[i].y + 4, WHITE);
                }
                
                
                // update snake body
                for (u32 i = snek->size - 1; i > 0; i--)
                {
                    snek->pos[i] = snek->pos[i - 1];
                }
                
            }
            
        }break;
        case LEVEL_OUTRO:
        {
            internal f32 timer;
            timer += delta;
            if(timer > 1/12.f)
            {
                RENDER_STATIC(screen, STATIC_MODE_BLACK);
                game->loading_bar.x = game->loading_bar.x < screen->width ? game->loading_bar.x + SNAKE_SPEED : 0;
                draw_bar(screen,game);
                
                
                timer = 0;
            }
            
            if(yk_input_is_key_tapped(input,YK_ACTION_ACCEPT))
            {
                //printf("%d\n",game->msg_index);
                
                send_msg(game,game->msg_index++);
                
                if(game->msg_index > MSG_BYE)
                {
                    exit(1);
                    // printf("bye");
                }
                
            }
            
        }break;
        
        default:
        {
            printf("ooga booga why is control here\n");
        }break;
    }
    
    
    
    // stupid debug
#if 0
    if (input->keys[YK_ACTION_HOLD_HANDS] == 1)
    {
        for (u32 i = 0; i < height; i++)
        {
            
            for (u32 j = 0; j < width; j++)
            {
                printf("%d ", screen->pixels[width * i + j]);
            }
            printf("\n");
        }
    }
#endif
}

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


internal void snake_mv(struct YkGame * game, struct YkInput *input)
{
    struct snake *snek = &game->snek;
    
    // printf("%d\n",game->wave);
    
    // snake position
    {
        if ((snek->pos[0].x < game->width) && snek->pos[0].x >= 0)
        {
            snek->pos[0].x += snek->dir.x * SNAKE_SPEED;
        }
        else
        {
            if (snek->pos[0].x > game->width - 1)
            {
                snek->pos[0].x = 0;
            }
            else
            {
                snek->pos[0].x = game->width - 1;
            }
        }
        
        if ((snek->pos[0].y < game->height) && snek->pos[0].y >= 0)
        {
            snek->pos[0].y += snek->dir.y * SNAKE_SPEED;
        }
        else
        {
            if (snek->pos[0].y > game->height - 1)
            {
                snek->pos[0].y = 0;
            }
            else
            {
                snek->pos[0].y = game->height - 1;
            }
        }
        
        //    printf("x%d y", snek.posX[0]);
        //    printf("%d\n", snek.posY[0]);
    }
}


// For you own sake. Don't look at the code after this line.
// It is metaprogramming sin.
// Don't say I didn't say I didn't say I didn't warn ya
// In a manner of speaking this is max re-usability.
// We will not speak of this manner.
// I will whip out a metaprogrammer to split code based on
// macros. I didn't want an if else inside such hot code for a situation
// where the if elsing was constant for the function.
// In fact, after this game jam. After acerola awards me for my ground breaking
// code and gameplay, I will make something to break code.
// I did this mostly for fun. It took 20 minutes at best.
// I don't even know why I am explaining this. Who even reads this stuff

#define _RENDER_STATIC_ONE    \
u32 width = screen->width; \
u32 height = screen->height; \
u32 *pixels = screen->pixels;   \
for (u32 i = 0; i < height; i++)    \
{                                   \
u32 pixel;                      \
for (u32 j = 0; j < width; j++) \
{                               


#define _STATIC_MODE_BASIC \
pixel = (0xFF << 24) | ((test_rand()) << 16) | (test_rand() << 8) | test_rand();

#define _STATIC_MODE_MIXED                                                                                 \
if (j % 2 == 0)                                                                     \
{ \
u32 randy = test_rand() * 1.5;              \
pixel = (0xFF << 24) | ((randy) << 16) | (randy << 8) | randy;  \
}   \
else    \
{   \
pixel = (0xFF << 24) | ((test_rand()) << 16) | (test_rand() << 8) | test_rand(); \
}

#define _STATIC_MODE_MIXED_2                                                                                 \
if (!(j % 5 == 0))                                                                     \
{ \
u32 randy = test_rand() * 1.5;              \
pixel = (0xFF << 24) | ((randy) << 16) | (randy << 8) | randy;  \
}   \
else    \
{   \
pixel = (0xFF << 24) | ((test_rand()) << 16) | (test_rand() << 8) | test_rand(); \
}       

#define _STATIC_MODE_HALF \
if (j > width / 2)\
{\
u32 randy = test_rand() * 1.5;\
pixel = (0xFF << 24) | ((randy) << 16) | (randy << 8) | randy;\
}\
else\
{\
pixel = (0xFF << 24) | ((test_rand()) << 16) | (test_rand() << 8) | test_rand();\
}

#define _STATIC_MODE_BLACK                                                       \
u32 randy = test_rand() * 1.5;                                   \
pixel = (0xFF << 24) | ((randy) << 16) | (randy << 8) | randy;   


#define _RENDER_STATIC_END        \
{\
u32 overlay = 0x44000000;\
\
u8 src_r = (pixel >> 16) & 0xFF;\
u8 src_g = (pixel >> 8) & 0xFF;\
u8 src_b = pixel & 0xFF;\
\
u8 dst_r = (overlay >> 16) & 0xFF;\
u8 dst_g = (overlay >> 8) & 0xFF;\
u8 dst_b = overlay & 0xFF;\
u8 dst_a = (overlay >> 24) & 0xFF;\
\
u8 new_r = (src_r * (255 - dst_a) + dst_r * dst_a) / 255;\
u8 new_g = (src_g * (255 - dst_a) + dst_g * dst_a) / 255;\
u8 new_b = (src_b * (255 - dst_a) + dst_b * dst_a) / 255;\
\
pixel = (0xFF << 24) | (new_r << 16) | (new_g << 8) | new_b;\
\
pixels[width * i + j] = pixel;\
}\
}\
}



void _render_static_STATIC_MODE_BASIC(struct render_buffer * screen)
{
    _RENDER_STATIC_ONE
        _STATIC_MODE_BASIC
        _RENDER_STATIC_END
}

void _render_static_STATIC_MODE_MIXED(struct render_buffer * screen)
{
    _RENDER_STATIC_ONE
        _STATIC_MODE_MIXED
        _RENDER_STATIC_END
}

void _render_static_STATIC_MODE_HALF(struct render_buffer * screen)
{
    _RENDER_STATIC_ONE
        _STATIC_MODE_HALF
        _RENDER_STATIC_END
}

void _render_static_STATIC_MODE_MIXED_2(struct render_buffer * screen)
{
    _RENDER_STATIC_ONE
        _STATIC_MODE_MIXED_2
        _RENDER_STATIC_END
}

void _render_static_STATIC_MODE_BLACK(struct render_buffer * screen)
{
    _RENDER_STATIC_ONE
        _STATIC_MODE_BLACK 
        _RENDER_STATIC_END
}

// metaprogramming sin ends here.


/*
void render_static(struct render_buffer * screen, STATIC_MODE mode)
{

    u32 width = screen->width;
    u32 height = screen->height;
    u32 *pixels = screen->pixels;

    for (u32 i = 0; i < height; i++)
    {
        u32 pixel;
        for (u32 j = 0; j < width; j++)
        {
    #if 0
            if (j % 2 == 0)
            {
                u32 randy = test_rand() * 1.5;
                pixel = (0xFF << 24) | ((randy) << 16) | (randy << 8) | randy;
            }
            else
            {
                pixel = (0xFF << 24) | ((test_rand()) << 16) | (test_rand() << 8) | test_rand();
            }
    #elif 1
            if (j > width / 2)
            {
                u32 randy = test_rand() * 1.5;
                pixel = (0xFF << 24) | ((randy) << 16) | (randy << 8) | randy;
            }
            else
            {
                pixel = (0xFF << 24) | ((test_rand()) << 16) | (test_rand() << 8) | test_rand();
            }

    #elif 0
            pixel = (0xFF << 24) | ((test_rand()) << 16) | (test_rand() << 8) | test_rand();

    #elif 0
            u32 randy = test_rand() * 1.5;
            pixel = (0xFF << 24) | ((randy) << 16) | (randy << 8) | randy;

    #endif

            // overlay thing
            // terrible terrible brute forced
            {
                u32 overlay = 0x44000000;

                u8 src_r = (pixel >> 16) & 0xFF;
                u8 src_g = (pixel >> 8) & 0xFF;
                u8 src_b = pixel & 0xFF;

                u8 dst_r = (overlay >> 16) & 0xFF;
                u8 dst_g = (overlay >> 8) & 0xFF;
                u8 dst_b = overlay & 0xFF;
                u8 dst_a = (overlay >> 24) & 0xFF;

                u8 new_r = (src_r * (255 - dst_a) + dst_r * dst_a) / 255;
                u8 new_g = (src_g * (255 - dst_a) + dst_g * dst_a) / 255;
                u8 new_b = (src_b * (255 - dst_a) + dst_b * dst_a) / 255;

                pixel = (0xFF << 24) | (new_r << 16) | (new_g << 8) | new_b;

                pixels[width * i + j] = pixel;
            }
        }
    }
}
*/
