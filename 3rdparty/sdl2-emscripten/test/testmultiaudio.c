/*
  Copyright (C) 1997-2014 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely.
*/
#include "SDL.h"

#include <stdio.h> /* for fflush() and stdout */

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

static SDL_AudioSpec spec;
static Uint8 *sound = NULL;     /* Pointer to wave data */
static Uint32 soundlen = 0;     /* Length of wave data */

typedef struct
{
    SDL_AudioDeviceID dev;
    int soundpos;
    volatile int done;
} callback_data;

callback_data cbd[64];

void SDLCALL
play_through_once(void *arg, Uint8 * stream, int len)
{
    callback_data *cbd = (callback_data *) arg;
    Uint8 *waveptr = sound + cbd->soundpos;
    int waveleft = soundlen - cbd->soundpos;
    int cpy = len;
    if (cpy > waveleft)
        cpy = waveleft;

    SDL_memcpy(stream, waveptr, cpy);
    len -= cpy;
    cbd->soundpos += cpy;
    if (len > 0) {
        stream += cpy;
        SDL_memset(stream, spec.silence, len);
        cbd->done++;
    }
}

void
loop()
{
    if(cbd[0].done) {
        emscripten_cancel_main_loop();
        SDL_PauseAudioDevice(cbd[0].dev, 1);
        SDL_CloseAudioDevice(cbd[0].dev);
        SDL_FreeWAV(sound);
        SDL_Quit();
    }
}

static void
test_multi_audio(int devcount)
{
    int keep_going = 1;
    int i;

    if (devcount > 64) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Too many devices (%d), clamping to 64...\n",
                devcount);
        devcount = 64;
    }

    spec.callback = play_through_once;

    for (i = 0; i < devcount; i++) {
        const char *devname = SDL_GetAudioDeviceName(i, 0);
        SDL_Log("playing on device #%d: ('%s')...", i, devname);
        fflush(stdout);

        SDL_memset(&cbd[0], '\0', sizeof(callback_data));
        spec.userdata = &cbd[0];
        cbd[0].dev = SDL_OpenAudioDevice(devname, 0, &spec, NULL, 0);
        if (cbd[0].dev == 0) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Open device failed: %s\n", SDL_GetError());
        } else {
            SDL_PauseAudioDevice(cbd[0].dev, 0);
#ifdef __EMSCRIPTEN__
            emscripten_set_main_loop(loop, 0, 1);
#else
            while (!cbd[0].done);
                SDL_Delay(100);
#endif
            SDL_Log("done.\n");
        }
    }
}


int
main(int argc, char **argv)
{
    int devcount = 0;

  /* Enable standard application logging */
    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

    /* Load the SDL library */
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s\n", SDL_GetError());
        return (1);
    }

    SDL_Log("Using audio driver: %s\n", SDL_GetCurrentAudioDriver());

    devcount = SDL_GetNumAudioDevices(0);
    if (devcount < 1) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Don't see any specific audio devices!\n");
    } else {
        if (argv[1] == NULL) {
            argv[1] = "sample.wav";
        }

        /* Load the wave file into memory */
        if (SDL_LoadWAV(argv[1], &spec, &sound, &soundlen) == NULL) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load %s: %s\n", argv[1],
                    SDL_GetError());
        } else {
            test_multi_audio(devcount);
            SDL_FreeWAV(sound);
        }
    }

    SDL_Quit();
    return 0;
}
