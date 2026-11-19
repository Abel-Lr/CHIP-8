#include <SDL2/SDL.h>
#include "platform.h"
#include "chip8.h"

int platform_init(Platform *platform, const char *title, int w, int h)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init failed : %s", SDL_GetError());
        return -1;
    }
    platform->window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        w,
        h,
        0);
    if (!platform->window)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindow failed : %s", SDL_GetError());
        return -1;
    }

    platform->renderer = SDL_CreateRenderer(platform->window, -1, 0);
    if (!platform->renderer)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateRenderer failed : %s", SDL_GetError());
        return -1;
    }
    return 0;
}

void platform_destroy(Platform *platform)
{
    if (platform->renderer)
    {
        SDL_DestroyRenderer(platform->renderer);
        platform->renderer = NULL;
    }
    if (platform->window)
    {
        SDL_DestroyWindow(platform->window);
        platform->window = NULL;
    }
    SDL_Quit();
}

void platform_render(Platform *platform, const Chip8 *chip8)
{
    SDL_SetRenderDrawColor(platform->renderer, 0x14, 0x14, 0x14, 0xff);
    SDL_RenderClear(platform->renderer);
    SDL_SetRenderDrawColor(platform->renderer, 0x32, 0xff, 0x66, 0xff);
    for (int y = 0; y < CHIP8_SCREEN_HEIGHT; y++)
    {
        for (int x = 0; x < CHIP8_SCREEN_WIDTH; x++)
        {
            if (chip8->screen[y][x])
            {
                SDL_Rect pixel = {.h = PLATFORM_PIXEL_SCALE, .w = PLATFORM_PIXEL_SCALE, .x = x * PLATFORM_PIXEL_SCALE, .y = y * PLATFORM_PIXEL_SCALE};
                SDL_RenderFillRect(platform->renderer, &pixel);
            }
        }
    }
    SDL_RenderPresent(platform->renderer);
}