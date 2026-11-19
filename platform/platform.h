#ifndef PLATFORM_H
#define PLATFORM_H
#include <SDL2/SDL.h>
#include "chip8.h"
#include <stdbool.h>

#define PLATFORM_PIXEL_SCALE 10
#define PLATFORM_TITLE "CHIP-8"
#define PLATFORM_HOLD_QUIT_DURATION_MS 1000
#define PLATFORM_QUIT_MAX_DOTS 3

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    int theme_index;
    bool escape_held;
    Uint32 escape_press_time;
} Platform;

int platform_init(Platform *platform, int w, int h);

void platform_destroy(Platform *platform);

void platform_render(Platform *platform, const Chip8 *chip8);

void platform_next_theme(Platform *platform);

void platform_previous_theme(Platform *platform);

bool platform_process_events(Platform *platform, Chip8 *chip8);
#endif