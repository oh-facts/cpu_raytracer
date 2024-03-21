#ifndef YK_SDL2_INCLUDE_H
#define YK_SDL2_INCLUDE_H

//what the fuck man. sdl2 windows is so fuck. sdl3 has so much nicer api but sdl3 software rendering is fuck
// FUCK MICROSOFT (John Malkovitch voice)

//https://github.com/libsdl-org/SDL/issues/9221

#ifdef _WIN32
#define SDL_MAIN_HANDLED
#include <SDL.h>
#else
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#endif

#endif