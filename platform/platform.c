#include <SDL2/SDL.h>
#include "platform.h"

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

static void draw_centered_rect(Platform *platform, int rect_w, int rect_h)
{
    int screen_w, screen_h;
    SDL_GetWindowSize(platform->window, &screen_w, &screen_h);
    SDL_Rect rect = {.x = screen_w / 2 - rect_w / 2, .y = screen_h / 2 - rect_h / 2, .w = rect_w, .h = rect_h};
    SDL_RenderFillRect(platform->renderer, &rect);
}

void platform_render(Platform *platform)
{
    SDL_SetRenderDrawColor(platform->renderer, 0x0, 0x0, 0x0, 0xff);
    SDL_RenderClear(platform->renderer);
    SDL_SetRenderDrawColor(platform->renderer, 0xff, 0xff, 0xff, 0xff);
    draw_centered_rect(platform, 10, 10);
    SDL_RenderPresent(platform->renderer);
}