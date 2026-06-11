#ifndef NEW_FILE_C
#define NEW_FILE_C
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL.h>

int init_audio();
void pause_resume_Music();
void stop_audio();
int play_audio(Mix_Music *music, int loops);
int change_audio_volume();
void play_from_start();
void fade_out();
int audio_control(Mix_Music *music);
int menu();


#endif