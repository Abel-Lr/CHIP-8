#ifndef AUDIO_H
#define AUDIO_H
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>

#define AUDIO_SAMPLE_RATE 44100
#define AUDIO_SAMPLES 512
#define AUDIO_VOLUME (INT16_MAX / 10)
#define AUDIO_BEEP_FREQ 660
#define AUDIO_SAMPLE_PER_FRAME (AUDIO_SAMPLE_RATE / 60) // 60 frames per second

typedef struct
{
    SDL_AudioDeviceID device;
    uint32_t sample_count;
} Audio;

int audio_init(Audio *audio);
void audio_update(Audio *audio, bool beep);
void audio_destroy(Audio *audio);
#endif