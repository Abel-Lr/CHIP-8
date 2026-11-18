#ifndef PLATFORM_H
#define PLATFORM_H
#include <SDL2/SDL.h>

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
} Platform;

int platform_init(Platform *platform, const char *title, int w, int h);

void platform_destroy(Platform *platform);

void platform_render(Platform *platform);
#endif