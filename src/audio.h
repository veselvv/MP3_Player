#ifndef AUDIO_H
#define AUDIO_H
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL.h>

int init_audio();
void pause_resume_Music();
void stop_audio();
int play_audio(Mix_Music *music, int loops);
int change_audio_volume();
void play_from_start();
void fade_out();



#endif