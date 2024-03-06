#ifndef YK_OS_H
#define YK_OS_H

#include <yk_common.h>
#include <yk_math.h>
enum YK_KEY
{
	YK_KEY_LSHIFT	= 0xA0,
	YK_KEY_LCTRL	= 0xA2,
	YK_KEY_ESC		= 0x1B,

	YK_KEY_F1		= 0x70,
	YK_KEY_F2		= 0x71,
	YK_KEY_F3		= 0x72,
	YK_KEY_F4		= 0x73,
	YK_KEY_F5		= 0x74,
	YK_KEY_F6		= 0x75,
	YK_KEY_F7		= 0x76,
	YK_KEY_F8		= 0x77,
	YK_KEY_F9		= 0x78,
	YK_KEY_F10		= 0x79,
	YK_KEY_F11		= 0x7A,
	YK_KEY_F12		= 0x7B,
};

struct YkKeyState
{
	b8 _cur[256];
	b8 _old[256];
};

struct YkMouseClickState
{
	//0 - left, 1 - right, 2 - middle
	b8 _cur[3];
	b8 _old[3];
};

enum YK_MOUSE_BUTTON
{
	YK_MOUSE_BUTTON_LEFT,
	YK_MOUSE_BUTTON_RIGHT,
	YK_MOUSE_BUTTON_MIDDLE,
};

typedef enum YK_MOUSE_BUTTON YK_MOUSE_BUTTON;

struct YkMousePosState
{
	/*
		cur and old don't serve purpose anymore
		since  rel is  them but better. It will 
		remain  however,  since it is  part  of 
		family  now  ( I'm joking, it has minor 
		use)
	*/
	v2 cur;
	v2 old;
	v2 rel;
};

YK_API b8 yk_input_is_key_tapped(struct YkKeyState* state, u32 key);
YK_API b8 yk_input_is_key_held(struct YkKeyState* state, u32 key);
YK_API b8 yk_input_is_key_released(struct YkKeyState* state, u32 key);
YK_API b8 yk_input_is_click(struct YkMouseClickState* state, YK_MOUSE_BUTTON button);
YK_API v2 yk_input_mouse_mv(struct YkMousePosState* state);
YK_API void yk_show_cursor(b8 flag);
YK_API void yk_clip_cusor(void* win_handle, b8 flag);



struct win_data
{
	//ToDo(facts) 12/31 0548: Make these flags a bitmask 
	//			  1/17: I hate this struct I made. I hate it I hate it. 
	//					I should have just left this stuff entirely inside YkWindow.
	//					I don't remember why I separated it.
	b8 is_resized;
	b8 is_minimized;
	b8 is_running;
	i32 size_x;
	i32 size_y;
};

struct YkWindow
{
	void * win_handle;
	struct win_data win_data;
	struct YkKeyState keys;
	struct YkMouseClickState clicks;
	struct YkMousePosState mouse_pos;

};

YK_API void yk_innit_window(struct YkWindow* window);
YK_API void yk_window_poll();
YK_API void yk_window_update(struct YkWindow* window);
YK_API void yk_free_window(struct YkWindow* window);

struct YkTimeState;

YK_API void yk_time_begin();
YK_API void yk_time_end();

#endif // !YK_WIN32_WINDOW_H
