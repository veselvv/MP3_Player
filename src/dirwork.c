#include <stdlib.h>
#include <stdio.h>  
#include <dirent.h>
#include <string.h>


int wrkdir(const char *dirname){
    DIR *dir = opendir(dirname);
    struct dirent *entry;
    if (dir == NULL){
        perror("opendir"); 
        return 1;
    }

    while(((entry = readdir(dir))!=NULL)){

        if(strcmp(entry->d_name, ".")==0 || strcmp(entry->d_name, "..")==0){
            continue;
        }
        if(entry->d_name[0]!='.'){
            if(strstr(entry->d_name, ".mp3")!=NULL){
                printf("%s\n", entry->d_name);
            }
        }
    }
    closedir(dir);
    return 0;
}


