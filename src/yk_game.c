#include <yk_game.h>

#define speed 1

internal u32 g_seed = 42;

YK_API void yk_innit_game(struct YkGame *game)
{
    struct snake *snek = &game->snek;

    snek->size = 5;
    snek->dir = (v2i){1, 0};

    for (i32 i = 0, size = snek->size; i < size; i++)
    {
        snek->pos[i] = (v2i){0, 50};
    }
}

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

    if ((snek->pos[0].x < screen->width) && snek->pos[0].x >= 0)
    {
        snek->pos[0].x += snek->dir.x;
    }
    else
    {
        if (snek->pos[0].x > (i32)screen->width - 1)
        {
            snek->pos[0].x = 0;
        }
        else
        {
            snek->pos[0].x = screen->width - 2;
        }
    }

    if ((snek->pos[0].y < screen->height) && snek->pos[0].y >= 0)
    {
        snek->pos[0].y += snek->dir.y;
    }
    else
    {
        if (snek->pos[0].y > (i32)screen->height - 1)
        {
            snek->pos[0].y = 0;
        }
        else
        {
            snek->pos[0].y = screen->height - 2;
        }
    }

    //    printf("x%d y", snek.posX[0]);
    //    printf("%d\n", snek.posY[0]);

    if (game->timer > 1 / 12.f)
    {
        game->timer = 0;
        u32 width = screen->width;
        u32 height = screen->height;
        u32 *pixels = screen->pixels;

        for (u32 i = 0; i < height; i++)
        {

            for (u32 j = 0; j < width; j++)
            {
#if 1
                if (j % 2 == 0)
                {
                    u32 randy = test_rand() * 1.5;
                    pixels[width * i + j] = (0xFF << 24) | ((randy) << 16) | (randy << 8) | randy;
                }
                else
                {
                    pixels[width * i + j] = (0xFF << 24) | ((test_rand()) << 16) | (test_rand() << 8) | test_rand();
                }
#elif 0
                if (j > width / 2)
                {
                    u32 randy = test_rand() * 1.5;
                    pixels[width * i + j] = (0xFF << 24) | ((randy) << 16) | (randy << 8) | randy;
                }
                else
                {
                    pixels[width * i + j] = (0xFF << 24) | ((test_rand()) << 16) | (test_rand() << 8) | test_rand();
                }

#elif 0
                pixels[width * i + j] = (0xFF << 24) | ((test_rand()) << 16) | (test_rand() << 8) | test_rand();

#elif 0
                u32 randy = test_rand() * 1.5;
                pixels[width * i + j] = (0xFF << 24) | ((randy) << 16) | (randy << 8) | randy;

#endif
            }
        }

        // need alpha blending for this
        // draw_rect(screen, 0, 0, screen->width / 2, screen->height / 2, 0x000000FF);

        // player
        // draw_rect(screen, 0 , 0, screen->width, screen->height, 0x09000000);
        //  snek.posX[0] = game->pos_x;
        //   snek.posY[0] = game->pos_y;

        for (u32 i = 0; i < snek->size; i++)
        {
            draw_rect(screen, snek->pos[i].x, snek->pos[i].y, snek->pos[i].x + 4,  snek->pos[i].y + 4, 0xFFFFFFFF);
        }

        for (u32 i = snek->size - 1; i > 0; i--)
        {
            snek->pos[i] = snek->pos[i-1];
        }
    }

    // ToDo(facts): Just accept rect. I dont want to do this disco with variables

    // printf("[%d %d]\n", game->pos_x, game->pos_y);

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
