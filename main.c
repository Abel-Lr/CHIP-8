#include <stdlib.h>

#include "platform.h"

int main(int argc, char **argv) {
    SDL_Event event;
    char running = 0x1;
    Platform platform = {0};
    if(platform_init(&platform, "App", 640, 320) < 0) {
        platform_destroy(&platform);
        return EXIT_FAILURE;
    }
    while(running) {
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    SDL_Log("Quitting...");
                    running = 0x0;
                    break;
                default:
                    SDL_Log("Unhandled Event : 0x%x", event.type);
                    break;
            }
        }
        platform_render(&platform);
        SDL_Delay(16); // 60fps
    }
    platform_destroy(&platform);
    return EXIT_SUCCESS;
}