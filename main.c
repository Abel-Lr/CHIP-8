#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "platform.h"
#include "chip8.h"

#define CYCLES_PER_FRAME 50

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <rom>\n", argv[0]);
        return EXIT_FAILURE;
    }
    const char *rom_path = argv[1];
    Chip8 chip8 = {0};
    chip8_init(&chip8);
    if (chip8_load_rom(&chip8, rom_path) < 0)
    {
        return EXIT_FAILURE;
    }

    Platform platform = {0};
    if (platform_init(
            &platform,
            CHIP8_SCREEN_WIDTH * PLATFORM_PIXEL_SCALE,
            CHIP8_SCREEN_HEIGHT * PLATFORM_PIXEL_SCALE) < 0)
    {
        platform_destroy(&platform);
        return EXIT_FAILURE;
    }
    while (platform_process_events(&platform, &chip8))
    {
        for (int i = 0; i < CYCLES_PER_FRAME; i++)
            chip8_cycle(&chip8);
        platform_render(&platform, &chip8);
        SDL_Delay(16); // 60fps
        chip8_update_timers(&chip8);
    }
    platform_destroy(&platform);
    return EXIT_SUCCESS;
}