#ifndef PLATFORM_H
#define PLATFORM_H
#include <SDL2/SDL.h>
#include "chip8.h"

#define PLATFORM_PIXEL_SCALE 10

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
} Platform;

int platform_init(Platform *platform, const char *title, int w, int h);

void platform_destroy(Platform *platform);

void platform_render(Platform *platform, const Chip8 *chip8);
#endif