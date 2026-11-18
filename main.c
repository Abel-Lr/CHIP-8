#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "platform.h"
#include "chip8.h"

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

    bool running = true;

    SDL_Event event;
    Platform platform = {0};
    if (platform_init(&platform, "App", 640, 320) < 0)
    {
        platform_destroy(&platform);
        return EXIT_FAILURE;
    }
    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                SDL_Log("Quitting...");
                running = false;
                break;
            default:
                break;
            }
        }
        platform_render(&platform);
        SDL_Delay(16); // 60fps
    }
    platform_destroy(&platform);
    return EXIT_SUCCESS;
}