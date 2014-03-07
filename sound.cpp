/*
 * Roguelike Engine in C++
 *
 * Sound specific stuff. Not very elegant.
 *
 * Copyright (C) rkaid <rkaidstudios@gmail.com> 2013, 2014
 * 
 * See LICENSE.txt for licensing information.
 *
 */

using namespace std;

#include <iostream>
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"

#include "actor.h"
#include "player.h"
#include "common.h"
#include "sound.h"

struct sound_def sound_defs[NUM_SOUNDS] = {
        //{ SOUND_EFFECT_CREAKING, effect, "sound/105288__mbezzola__knarrender-holzboden-in-orgel.wav"},
};
        
SoundEngine::SoundEngine()
{
        audio_rate = 44100;
        audio_format = AUDIO_S16SYS;
        audio_channels = 2;
        audio_buffers = 4096;
        sounds_count = 0;
}

SoundEngine::~SoundEngine()
{
        Mix_CloseAudio();
        for(int i = 0; i < NUM_SOUNDS; i++) {
                if(sound_defs[i].type == effect)
                        Mix_FreeChunk(s[i].chunk);
                if(sound_defs[i].type == music)
                        Mix_FreeMusic(s[i].music);
        }

        SDL_Quit();
}

/*void SoundEngine::load_file(const char *filename)
{
        s[sound_defs].chunk = Mix_LoadWAV(filename);
        if(s[sounds_count].chunk == NULL) {
                fprintf(stderr, "Unable to load WAV file: %s\n", Mix_GetError());
        }

        sounds_count++;
}*/

void SoundEngine::load_sound_def(int i)
{
        if(sound_defs[i].type == effect) {
                s[i].chunk = Mix_LoadWAV(sound_defs[i].filename);
                sounds_count++;
                if(s[i].chunk == NULL) {
                        fprintf(stderr, "Unable to load WAV file %s: %s\n", sound_defs[i].filename, Mix_GetError());
                        sounds_count--;
                }
        } else if(sound_defs[i].type == music) {
                s[i].music = Mix_LoadMUS(sound_defs[i].filename);
                sounds_count++;
                if(!s[i].music) {
                        dbg("Failed to load music file %s: %s\n", sound_defs[i].filename, Mix_GetError());
                        sounds_count--;
                }
        }
}

void SoundEngine::load_all_files()
{
        int i;
        for(i = 0; i < NUM_SOUNDS; i++) {
                load_sound_def(i);
        }
}

void SoundEngine::play_music(int sound)
{
        s[sound].channel = Mix_PlayMusic(s[sound].music, 1);
        if(s[sound].channel == -1) {
                fprintf(stderr, "Unable to play music %d: %s\n", sound, Mix_GetError());
        }
}

void SoundEngine::play_sound(int sound)
{
        if(sound > sounds_count) {
                dbg("Tried to play unloaded sound!");
                return;
        }
        s[sound].channel = Mix_PlayChannel(-1, s[sound].chunk, 0);
        if(s[sound].channel == -1) {
                fprintf(stderr, "Unable to play WAV file: %s\n", Mix_GetError());
        }

}

void SoundEngine::play_sound(int sound, int ms)
{
        if(sound > sounds_count) {
                dbg("Tried to play unloaded sound!");
                return;
        }
        s[sound].channel = Mix_FadeInChannel(-1, s[sound].chunk, 0, ms);
        if(s[sound].channel == -1) {
                fprintf(stderr, "Unable to play WAV file: %s\n", Mix_GetError());
        }

}

void SoundEngine::play_sound_infinite_loop(int sound)
{
        if(sound > sounds_count) {
                dbg("Tried to play unloaded sound!");
                return;
        }
        s[sound].channel = Mix_PlayChannel(-1, s[sound].chunk, -1);
        if(s[sound].channel == -1) {
                fprintf(stderr, "Unable to play WAV file: %s\n", Mix_GetError());
        }
}

void SoundEngine::pause_sound(int sound)
{
        Mix_Pause(s[sound].channel);
}

void SoundEngine::pause_sound(int sound, int ms)
{
        Mix_FadeOutChannel(s[sound].channel, ms);
}

void SoundEngine::set_sound_volume(int sound, int volume)
{
        Mix_VolumeChunk(s[sound].chunk, volume);
}

int SoundEngine::initialize()
{
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
                fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
                return 1;
        }

        if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0) {
                fprintf(stderr, "Unable to initialize audio: %s\n", Mix_GetError());
                exit(1);
        }

        return 0;
}
