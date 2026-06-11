#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdbool.h>

#include "dirwork.h"


#include <unistd.h>   // для getcwd
#include <sys/stat.h>


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


int play_audio(Mix_Music *music, int loops, int *runing){
    if(!music){
        printf("Error: play_audio: Invalid function arg\n");
        return -1;
    }
    if(Mix_PlayMusic(music, loops) == -1){
        printf("Error: err with play music\n");
        return -1;
    }
    while(Mix_PlayingMusic()){

    }
    return 0;
}


int change_audio_volume(){
    int level;
    printf("Введите уровень громкости(0 - 128): ");
    scanf("%d", &level);
    while(getchar()!='\n');
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

int audio_control(Mix_Music *music){
    if(!music){
        printf("Invalid function arg\n");
        return -1;
    }
    int choice;
    int was_stoped;
    while (Mix_PlayingMusic()){
        printf("1 - Pause/Resume\n"
                "2 - Воспроизвести сначала\n"
                "3 - Установить громкость\n"
                "4 - Остановить трек\n"
                "5 - Фейд затухание\n"
                "6 - Фейд увеличение\n");
        
        choice = getchar();
        while(getchar()!='\n');
        switch (choice){
        case '1':
            pause_resume_Music();
            break;
        case '2':
            play_from_start();
            break;
        case '3':
            change_audio_volume();
            break;
        case '4':
            stop_audio();
            was_stoped = 1;
            return was_stoped;
            break;
        case '5':
            fade_out();
            break;
        default:
            printf("Invalid choice\n");
            break;
        };
    }
    return was_stoped;
}



int menu(){
    int running = 1;
    Mix_Music *music;
    while (running){
        printf("Что вы хотите сделать?\n"
                "1 - Воспроизвести трек\n"
                "2 - Завершить работу\n");

        int choice = getchar();
        switch (choice){
            case '1':
                printf("Доступные треки:\n");
                wrkdir("../tracks/");
                char filename[256];
                scanf("%s", filename);
                music = Mix_LoadMUS(filename);
                if(!music){
                    printf("Error: Could not open audio file\n");
                    break;
                }
                if(Mix_PlayMusic(music,0)==-1){
                    printf("Error: %s", Mix_GetError());
                    return 1;
                }else{
                    while(getchar()!='\n');
                    if(audio_control(music)){ printf("Трек был остановлен вручную\n");}
                    else{printf("Трек закончился самостоятельно\n");}
                
                }
                break;

            case '2':
                if(music){
                    Mix_FreeMusic(music);
                }
                running = 0;
                break;

            default:
                break;
        }
    }
    return 0;
    
}


int main(int argc, char *argv[]){
  

    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Текущая рабочая директория: %s\n", cwd);
    } else {
        perror("getcwd");
    }

    // Проверка существования ../tracks
    struct stat st;
    if (stat("../tracks", &st) == 0) {
        if (S_ISDIR(st.st_mode))
            printf("Папка ../tracks существует\n");
        else
            printf("../tracks не директория\n");
    } else {
        perror("stat");
    }
    if(init_audio()!=0){
        return 1;
    }
    menu();


    return 0;
}