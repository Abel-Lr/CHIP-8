#include "audio.h"
#include <stdbool.h>

int audio_init(Audio *audio)
{
    audio->device = 0;
    audio->sample_count = 0;

    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO, "SDL_InitSubSystem failed : %s", SDL_GetError());
        audio_destroy(audio);
        return -1;
    }
    SDL_AudioSpec spec = {0};
    spec.freq = AUDIO_SAMPLE_RATE;
    spec.format = AUDIO_S16SYS;
    spec.channels = 1;
    spec.samples = AUDIO_SAMPLES;
    audio->device = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0);
    if (audio->device == 0)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO, "SDL_OpenAudioDevice failed : %s", SDL_GetError());
        audio_destroy(audio);
        return -1;
    }
    SDL_PauseAudioDevice(audio->device, 0);
    return 0;
}

void audio_update(Audio *audio, bool beep)
{
    if (audio->device == 0)
        return;

    if (!beep)
    {
        SDL_ClearQueuedAudio(audio->device);
        return;
    }

    int16_t samples[AUDIO_SAMPLE_PER_FRAME];
    uint32_t period = AUDIO_SAMPLE_RATE / AUDIO_BEEP_FREQ;
    for (int i = 0; i < AUDIO_SAMPLE_PER_FRAME; i++)
    {
        if ((audio->sample_count % period) < (period / 2))
            samples[i] = -AUDIO_VOLUME;
        else
            samples[i] = AUDIO_VOLUME;
        audio->sample_count++;
    }
    SDL_QueueAudio(audio->device, samples, sizeof(samples));
}

void audio_destroy(Audio *audio)
{
    if (audio->device != 0)
    {
        SDL_CloseAudioDevice(audio->device);
        audio->device = 0;
    }
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}