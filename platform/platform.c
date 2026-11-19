#include <SDL2/SDL.h>
#include "platform.h"
#include "chip8.h"

typedef struct
{
    SDL_Color bg;
    SDL_Color fg;
    const char *name;
} Theme;

static const Theme themes[] = {
    {.name = "Classic", .bg = {.r = 0x00, .g = 0x00, .b = 0x00, .a = 0xFF}, .fg = {.r = 0xFF, .g = 0xFF, .b = 0xFF, .a = 0xFF}},
    {.name = "Phosphor", .bg = {.r = 0x0A, .g = 0x14, .b = 0x0A, .a = 0xFF}, .fg = {.r = 0x33, .g = 0xFF, .b = 0x33, .a = 0xFF}},
    {.name = "Gameboy", .bg = {.r = 0x0F, .g = 0x38, .b = 0x0F, .a = 0xFF}, .fg = {.r = 0x9B, .g = 0xBC, .b = 0x0F, .a = 0xFF}},
    {.name = "Nokia 3310", .bg = {.r = 0x84, .g = 0x9C, .b = 0x78, .a = 0xFF}, .fg = {.r = 0x1E, .g = 0x24, .b = 0x1B, .a = 0xFF}},
    {.name = "Commodore 64", .bg = {.r = 0x3E, .g = 0x31, .b = 0xA2, .a = 0xFF}, .fg = {.r = 0x7B, .g = 0x71, .b = 0xD6, .a = 0xFF}},
};

static const int nb_themes = sizeof(themes) / sizeof(themes[0]);

static void update_window_title(Platform *platform)
{
    char new_title[256];
    snprintf(
        new_title,
        sizeof(new_title),
        "%s - %s",
        PLATFORM_TITLE,
        themes[platform->theme_index].name);
    SDL_SetWindowTitle(platform->window, new_title);
}

int platform_init(Platform *platform, int w, int h)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init failed : %s", SDL_GetError());
        return -1;
    }
    platform->window = SDL_CreateWindow(
        PLATFORM_TITLE,
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
    platform->theme_index = 0;
    platform->escape_held = false;
    platform->escape_press_time = 0;
    update_window_title(platform);
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
    const Theme *theme = &themes[platform->theme_index];
    SDL_SetRenderDrawColor(
        platform->renderer,
        theme->bg.r,
        theme->bg.g,
        theme->bg.b,
        theme->bg.a);
    SDL_RenderClear(platform->renderer);
    SDL_SetRenderDrawColor(
        platform->renderer,
        theme->fg.r,
        theme->fg.g,
        theme->fg.b,
        theme->fg.a);
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

void platform_next_theme(Platform *platform)
{
    platform->theme_index = (platform->theme_index + 1) % nb_themes;
    update_window_title(platform);
}

void platform_previous_theme(Platform *platform)
{
    platform->theme_index = ((platform->theme_index - 1) % nb_themes + nb_themes) % nb_themes;
    update_window_title(platform);
}

static void show_quit_progress(Uint32 elapsed_time_held)
{
    const Uint32 dot_interval = PLATFORM_HOLD_QUIT_DURATION_MS / PLATFORM_QUIT_MAX_DOTS;
    int nb_points = elapsed_time_held / dot_interval + 1;
    if (nb_points > PLATFORM_QUIT_MAX_DOTS)
        nb_points = PLATFORM_QUIT_MAX_DOTS;
    printf("\rQuitting%.*s\033[K", nb_points, "...");
    fflush(stdout);
}

bool platform_process_events(Platform *platform, Chip8 *chip8)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            SDL_Log("Quitting...");
            return false;
        case SDL_KEYDOWN:
            if (event.key.repeat != 0)
                break;
            else
                switch (event.key.keysym.sym)
                {
                case SDLK_y:
                    platform_next_theme(platform);
                    break;
                case SDLK_t:
                    platform_previous_theme(platform);
                    break;
                case SDLK_ESCAPE:
                    platform->escape_held = true;
                    platform->escape_press_time = SDL_GetTicks();
                    break;
                default:
                    break;
                }
            break;
        case SDL_KEYUP:
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                platform->escape_held = false;
                printf("\r\033[K");
                fflush(stdout);
            }
            break;
        default:
            break;
        }
    }
    if (platform->escape_held)
    {
        Uint32 elapsed_time = SDL_GetTicks() - platform->escape_press_time;
        show_quit_progress(elapsed_time);
        if (elapsed_time >= PLATFORM_HOLD_QUIT_DURATION_MS)
        {
            printf("\n");
            return false;
        }
    }
    return true;
}