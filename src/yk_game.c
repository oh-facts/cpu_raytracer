#include <yk_game.h>
#include <yk_renderer.h>

YK_API void yk_innit_game(struct YkGame *game, struct render_buffer *screen);

YK_API void yk_update_and_render_game(struct render_buffer *screen, struct YkInput *input, struct YkGame *game, f32 delta);



internal u32 g_seed = 42;
internal u32 test_rand();
internal u32 lcg_rand();

#define SNAKE_SPEED 5
#include <string.h>

#define WHITE (0xFFFFFFFF)
#define BLACK (0xFF000000)

void load_level(struct YkGame* game);
internal void send_msg(struct YkGame* game, YKMSG msg);

void draw_candy_bg(struct YkGame* game, f32 delta);


internal void draw_rect_pos_scale(struct render_buffer *screen, v2i pos, v2i scale,u32 rgba);

#define V2I_FMT "%d %d"
#define V2I_(s) (s).x, (s.y)

internal void reset_apples(struct YkGame* game)
{
    for(u32 i = 0; i < MAX_APPLES; i ++)
    {
        game->apples[i] = (v2i){-100, -100};
    }
}

internal void death_screen(struct YkGame* game);

// Remember when calling this function, you want to factor in loading bar //height when deciding height. Because otherwise it might be at the same level //and then you collect the apples while loading and thats weird
internal void randomise_apples(struct YkGame* game ,u32 width, u32 height, u32 num)
{
    for(u32 i = 0; i < num; i ++)
    {
        game->apples[i] = (v2i){lcg_rand() % width,lcg_rand() % height};
    }
}

#define LOADING_BAR_X 4
#define LOADING_BAR_Y 50
#define LOADING_BAR_POS (v2i){LOADING_BAR_X, LOADING_BAR_Y};

internal void draw_bar(struct render_buffer* screen, struct YkGame *game)
{
    v2i _lb = game->loading_bar;
    u32 color = game->loading_bar_color;
    draw_rect(screen, _lb.x ,_lb.y, _lb.x + 4, _lb.y + 4, color);
    draw_rect(screen, _lb.x+5 ,_lb.y, _lb.x + 9, _lb.y + 4, color);
    draw_rect(screen, _lb.x+10 ,_lb.y, _lb.x + 14, _lb.y + 4, color);
    
}

internal void draw_apples(struct YkGame* game, struct render_buffer * screen, u32 color, u32 num_apples)
{
    for(u32 i = 0; i < num_apples; i ++)
    {
        draw_rect_pos_scale(screen,game->apples[i], (v2i){2,2},color);
    }
}

internal void snake_mv(struct YkGame * game, struct YkInput *input);

internal void snake_apple_collision(struct YkGame* game, const u32 apple_num, b8 grow)
{
    struct snake *snek = &game->snek;
    // check and eat apple. using broadphase SAT AABB. Might optmize with quadtrees
    for(u32 i = 0; i < apple_num; i ++)
    {
        if((abs(snek->pos[0].x - game->apples[i].x) < 4) && ((abs(snek->pos[0].y - game->apples[i].y) < 4)))
        {
            
            game->apples[i].x = -101; //lcg_rand() % screen->width;
            game->apples[i].y = -101; //lcg_rand() % screen->height;
            
            if(grow)
            {
                snek->pos[snek->size] = snek->pos[snek->size - 1];
                if(snek->size == SNAKE_MAX_SIZE - 1)
                {
                    printl("Undesired behaviour. Snake wont grow more because max length reached. I left this msg to be notified");
                }
                else
                {
                    snek->size ++;
                }
            }
            else
            {
                snek->size --;
            }
            
            game->num_apples --;
            game->eaten ++;
            //printl("Gulp %d",i);
        }
        
        //printl("%d: "V2I_FMT,i,V2I_(game->apples[i]));
    }
}
//ToDo(facts): Metaprogram this
//Deeps clone game data. Exists so I can save.
internal void game_data_clone(struct YkGame* dst, struct YkGame* src)
{
    dst->opa = src->opa;
    
    dst->loading_bar = src->loading_bar;
    dst->loading_bar_color = src->loading_bar_color;
    
    dst->snek        = src->snek;
    dst->eaten       = src->eaten;
    dst->wave        = src->wave;
    dst->msg_index   = src->msg_index;
    dst->msg_last    = src->msg_last;
    for(u32 i = 0; i < MAX_APPLES; i++)
    {
        dst->apples[i] = src->apples[i];
    }
    dst->num_apples = src->num_apples;
    
    dst->align_timer      = src->align_timer;
    dst->align_msg_flag   = src->align_msg_flag;
    
    dst->align_wait_timer = src->align_wait_timer;
    dst->align_wait_flag  = src->align_wait_flag;
    
    dst->level       = src->level;
    
    dst->timer       = src->timer;
    
    dst->bgm = src->bgm;
    dst->alert_sound = src->alert_sound;
    
    dst->width       = src->width;
    dst->height      = src->height;
    
    dst->saved       = src->saved;
    dst->last_msg    = src->last_msg;
    
    dst->arena       = src->arena;
    memcpy(dst->arena.base, src->arena.base,dst->arena.used);
    
    // why am I doing this? This is platform. game doesn't
    //care about this
    dst->_win        = src->_win;
    dst->platform_innit_audio = src->platform_innit_audio;
    dst->platform_play_audio = src->platform_play_audio;
    dst->platform_stop_audio = src->platform_stop_audio;
    dst->platform_set_title = src->platform_set_title;
    
}

internal void game_data_save(struct YkGame* game)
{
    printl("saving");
    game_data_clone(game->saved,game);
}

internal void game_data_restore(struct YkGame* game)
{
    printl("restoring");
    game_data_clone(game,game->saved);
    send_msg(game, game->last_msg);
}

internal void death_screen(struct YkGame* game)
{
    printl(":( DEAD DEAD");
    
    game_data_restore(game);
}

internal b8 snake_loading_bar_collision(struct snake* snek)
{
    if((abs(snek->pos[0].y - LOADING_BAR_Y) <= 6) && snek->dir.x == 1)
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
    game->last_msg  = msg;
    game->platform_play_audio(game->alert_sound);
    //printf("w");
}

void yk_innit_game(struct YkGame *game, struct render_buffer* buffer)
{
    
    game->bgm = game->platform_innit_audio("../res/song0.wav");
    game->alert_sound = game->platform_innit_audio("../res/GameAlert.wav");
    
    
    game->level = 0;
    game->width = 80;
    game->height = 60;
    //game->wave = SNAKE_WAVE_3;
    game->loading_bar_color = 0xFFFFFFFF;
    
    load_level(game);
    
    
    game->main.width = 80;
    game->main.height = 60;
    game->main.pixels = push_array(&game->arena, sizeof(u32), 80 * 60);
    
    {
        char* file_data = game->platform_read_file("../res/test.bmp",&game->scratch);
        game->rabbit = read_bitmap_file(file_data,&game->arena);
        
        game->scratch.used = 0;
    }
    
    {
        char* file_data = game->platform_read_file("../res/font.bmp",&game->scratch);
        game->welcome = read_bitmap_file(file_data,&game->arena);
        
        game->scratch.used = 0;
        
    }
    
    game->saved = push_struct(&game->arena, game);
    game_data_save(game);
    
    
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
            game->loading_bar = LOADING_BAR_POS;
        }break;
        case LEVEL_SNAKE:
        {
            reset_apples(game);
            
            game->num_apples = SNAKE_LEVEL_START_APPLE_NUM;
            randomise_apples(game,80, LOADING_BAR_Y  - 5, SNAKE_LEVEL_START_APPLE_NUM );
            
            struct snake *snek = &game->snek;
            snek->size = 3;
            snek->dir = (v2i){1, 0};
            snek->pos[0] = LOADING_BAR_POS;
            
            game->platform_set_title(game->_win, messages[MSG_SNAKE_TRYING_TO_CONN]);
            
        }break;
        case LEVEL_OUTRO:
        {
            printf("e");
            game->msg_index = MSG_OUTRO_1;
            send_msg(game,game->msg_index++);
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


void draw_rect_pos_scale(struct render_buffer *screen, v2i pos, v2i scale,u32 rgba)
{
    draw_rect(screen, pos.x - scale.x, pos.y - scale.y, pos.x + scale.x, pos.y + scale.y, rgba);
}

b8 yk_input_is_key_tapped(struct YkInput *state, u32 key)
{
    return state->keys[key] && !state->keys_old[key];
}

b8 yk_input_is_key_held(struct YkInput *state, u32 key)
{
    return state->keys[key];
}

void yk_update_and_render_game(struct render_buffer *screen, struct YkInput *input, struct YkGame *game, f32 delta)
{
    
    game->timer += delta;
    
    if (yk_input_is_key_tapped(input, YK_ACTION_SAVE))
    {
        game_data_save(game);
    }
    else if (yk_input_is_key_tapped(input, YK_ACTION_RESTORE))
    {
        game_data_restore(game);
    }
    
    switch (game->level)
    {
        case LEVEL_INTRO:
        {
            game->loading_bar.x = game->loading_bar.x < game->main.width ? game->loading_bar.x + 1 : 0;
            if(yk_input_is_key_tapped(input,YK_ACTION_ACCEPT))
            {
                if(game->msg_index == MSG_INTRO_5)
                {
                    game->platform_play_audio(game->bgm);
                }
                
                if(game->msg_index == MSG_INTRO_6 + 1)
                {
                    game->level ++;
                    load_level(game);
                    break;
                }
                
                send_msg(game, game->msg_index++);
                
            }
            
            // sim speed
            if (game->timer > 1 / 12.f)
            {
                game->timer = 0;
                draw_candy_bg(game, delta);
                
                //loading bar
                if(game->msg_index > MSG_INTRO_3)
                {
                    draw_bar(&game->main,game);
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
            
            
            // sim world
            if (game->timer > 1 / 12.f)
            {
                game->timer = 0;
                
                switch (game->wave)
                {
                    
                    case SNAKE_WAVE_START:
                    {
                        //draw my life
                        snake_eat_self(snek);
                        draw_candy_bg(game, delta);
                        
                        // black pixel
                        draw_rect_pos_scale(&game->main, (v2i){3,10}, (v2i){4,4}, BLACK);
                        
                        
                        draw_apples(game,&game->main, WHITE,SNAKE_LEVEL_START_APPLE_NUM);
                        snake_mv(game,input);
                        // check and eat apple. using broadphase SAT AABB. Might optmize with quadtrees
                        snake_apple_collision(game, SNAKE_LEVEL_START_APPLE_NUM ,1);
                        
                        
                        if(snake_loading_bar_collision(snek))
                        {
                            if(game->last_msg != MSG_SNAKE_TRYING_TO_CONN)
                            {
                                send_msg(game,MSG_SNAKE_TRYING_TO_CONN);
                            }
                            
                            //ToDo(facts): rework
                            //delta * 6 is 1 second.
                            game->align_timer += delta * (6/3.f) * (((snek->size - 3)/(9 - 3.f)) * (180 - 3) + 3);
                            
                            if(game->align_timer > 180)
                            {
                                send_msg(game,MSG_SNAKE_ALIGN);
                                snek->pos[0] = (v2i){-1,-1};
                                game->wave ++;
                                break;
                            }
                            
                            
                        }
                        else
                        {
                            if(game->last_msg != MSG_SNAKE_LOST_CONN)
                            {
                                send_msg(game,MSG_SNAKE_LOST_CONN);
                            }
                            game->align_timer= 0;
                            
                        }
                        
                        
                        //printf("%d\n",game->wave);
                        
                        
                        
                    }break;
                    case SNAKE_WAVE_ALIGN_WAIT:
                    {
                        draw_candy_bg(game, delta);
                        game->align_wait_timer += delta * 4;
                        
                        
                        game->loading_bar.x = game->loading_bar.x < game->main.width ? game->loading_bar.x + SNAKE_SPEED : 0;
                        draw_bar(&game->main,game);
                        
                        if(game->align_wait_timer > 3 && !game->align_wait_flag)
                        {
                            game->align_wait_flag = 1;
                            //send_msg(game, MSG_SNAKE_3);
                            game->wave ++;
                            game->eaten = 0;
                            game->num_apples = SNAKE_LEVEL_DEAD_PIXEL_APPLE_NUM ;
                            reset_apples(game);
                            randomise_apples(game,80,LOADING_BAR_Y - 5, SNAKE_LEVEL_DEAD_PIXEL_APPLE_NUM);
                            snek->pos[0] = LOADING_BAR_POS;
                            break;
                        }
                        
                        //snake_apple_collision(game,apple_num_index[game->wave]);
                        
                        
                    }break;
                    case SNAKE_WAVE_DEAD_PIXELS:
                    {
                        draw_candy_bg(game, delta);
                        draw_apples(game,&game->main, WHITE,SNAKE_LEVEL_DEAD_PIXEL_APPLE_NUM);
                        snake_apple_collision(game,SNAKE_LEVEL_DEAD_PIXEL_APPLE_NUM,0);
                        snake_mv(game,input);
                        snake_eat_self(snek);
                        //ToDo(facts): puke
                        if(game->eaten == 1 && game->last_msg!= MSG_DEAD_PIXELS_1)
                        {
                            send_msg(game,MSG_DEAD_PIXELS_1);
                        }
                        else if(game->eaten == 2 && game->last_msg!= MSG_DEAD_PIXELS_2)
                        {
                            send_msg(game,MSG_DEAD_PIXELS_2);
                        }
                        else if(game->eaten == 3 && game->last_msg != MSG_DEAD_PIXELS_3)
                        {
                            send_msg(game,MSG_DEAD_PIXELS_3);
                        }
                        
                        if(snek->size == 0)
                        {
                            snek->size = 1;
                            game->level ++;
                            load_level(game);
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
                    draw_rect(&game->main, snek->pos[i].x, snek->pos[i].y, snek->pos[i].x + 4, snek->pos[i].y + 4, WHITE);
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
                draw_candy_bg(game,delta);
                game->loading_bar.x = game->loading_bar.x < game->main.width ? game->loading_bar.x + SNAKE_SPEED : 0;
                draw_bar(&game->main,game);
                
                
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
    
    //memcpy(screen->pixels,)
    
    
    u32 tv_w = 8;
    u32 tv_h = 6;
    u32 pad_w = 4; // 4 on each side
    u32 pad_h = 3;
    u32 win_w = 16;
    u32 win_h = 9;
    
    //tv render
    struct render_rect ren_rect= {0};
    
    ren_rect.h = screen->height - (screen->height /pad_h*1.f) * (0.6f);
    ren_rect.w = ren_rect.h * (tv_w*1.f/tv_h) ;
    ren_rect.x = (screen->width - ren_rect.w)/2;
    ren_rect.y = 0;
    
    blit_bitmap_scaled(screen, &game->main, &ren_rect);
    
    blit_bitmap_scaled(screen,(struct render_buffer*) &game->rabbit,&ren_rect);
    
    blit_bitmap_scaled(screen,(struct render_buffer*) &game->welcome,&ren_rect);
    
    //blit_bitmap_scaled(screen,(struct render_buffer*) &game->welcome,&ren_rect);
    
    
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


void draw_candy_bg(struct YkGame* game, f32 delta)
{
    u32 width = game->main.width;
    u32 height = game->main.height;
    u32 *pixels = game->main.pixels;
    for (u32 i = 0; i < height; i++)
    {                                  
        u32 pixel;                     
        for (u32 j = 0; j < width; j++)
        {
            game->opa +=delta * 100;
            game->opa = game->opa > 255 ? 0 : game->opa;
            /*printl("%d",opa);*/
            pixel = (0xFF << 24) | ( (100) << 16) | ((36 - game->opa) << 8) | game->opa - 12;
            
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

