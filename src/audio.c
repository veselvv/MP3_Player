#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "music_duration.h"
#include "dirwork.h"


int init_audio(){
    if(SDL_Init(SDL_INIT_AUDIO)!=0){
        printf("Error: could not init SDL_Audio\n");
        return 1;
    }
    int mix_flags = MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MID;
    int initialized = Mix_Init(mix_flags);
    if(!(initialized & MIX_INIT_MP3)){
        printf("Warning: MP3 не поддерживается\n");
        return 1;
    }
    if(Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 2048)<0){
        printf("Error: Init audio error: %s\n", Mix_GetError());
        return 1;
    }
    return 0;

}

void pause_resume_Music(){
    if(Mix_PausedMusic()){
        Mix_ResumeMusic();
    }else{
        Mix_PauseMusic();
    }
}





void stop_audio(){
    Mix_HaltMusic();
}


int play_audio(Mix_Music *music, int loops){
    if(!music){
        printf("Error: play_audio: Invalid function arg\n");
        return -1;
    }
    if(Mix_PlayMusic(music, loops) == -1){
        printf("Error: err with play music\n");
        return -1;
    }
    return 0;
}


int change_audio_volume(int level){
    if(level>=0 && level<=128){
        Mix_VolumeMusic(level);
        return 0;
    }else{
        printf("Введено невалидное значение громкости\n");
        return 1;
    }
}


void play_from_start(){
    
    Mix_RewindMusic();
}



void fade_out(){
    int ms;
    printf("Введите через сколько секунд должно быть затухание: ");
    scanf("%d", &ms);
    Mix_FadeOutMusic(ms*1000);
}


