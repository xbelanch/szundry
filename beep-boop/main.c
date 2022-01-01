#include <SDL2/SDL.h>

#define SAMPLE_RATE 48000
#define CHANNELS 2
#define SAMPLES (SAMPLE_RATE*2)

int main (int argc, char** argv)
{
    (void) argc;
    (void) argv;

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_AudioSpec want, got;
    want.freq = SAMPLE_RATE;
    want.format = AUDIO_F32;
    want.channels = CHANNELS;
    want.samples = 4096;
    want.callback = NULL;

    float* buffer = calloc(SAMPLES * CHANNELS, sizeof *buffer);
    for (int i = 0; i < SAMPLE_RATE; i++) {
        float t = (float)i / (float)SAMPLE_RATE;
        float f = sin(t*648*3.14159f) * t*(1-t);
        float g = sin(t*432*3.14159f) * t*(1-t);
        buffer[i*2] = f;
        buffer[i*2+1] = f;
        buffer[SAMPLE_RATE*2 + i*2] = g;
        buffer[SAMPLE_RATE*2 + i*2 + 1] = g;
    }

    SDL_AudioDeviceID dev = SDL_OpenAudioDevice(NULL, 0, &want, &got, 0);
    if (dev == 0) {
        SDL_Log("Failed to open audio: %s", SDL_GetError());
    } else {
        SDL_QueueAudio(dev, buffer, SAMPLES * CHANNELS * sizeof *buffer);
        SDL_PauseAudioDevice(dev, 0);
        SDL_Delay(2200);
        SDL_CloseAudioDevice(dev);
    }
    SDL_Quit();
    return 0;
}
