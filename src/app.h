#ifndef APP_H
#define APP_H
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL.h>
#include <gtk/gtk.h>

typedef enum{
    PAUSED,
    PLAYED,
    DISABLED,
    ON,
    OFF
}ButtonState;


typedef enum{
    KIRBY,
    BULBASAUR,
    MEWTWO,
    CLOUD
}GifPersons;


typedef struct{
    Mix_Music *music;
    double duration;
    char filename[512];
    char folderpath[512];
    int current_track_index;
    int last_track_index;
}MusikData;

typedef struct {
    GdkPixbufAnimation *anim;
    GdkPixbufAnimationIter *iter;
    GtkWidget *picture;
    guint timeout_id;
    gboolean paused; 
} GifData;

typedef struct{
    GifData *gdata;
    GtkWindow *main_window;
    GtkWidget *main_play_button;
    GtkWidget *main_play_button_icon;
    GtkWidget *main_prev_button;
    GtkWidget *main_scale;
    GtkWidget *main_label_time;
    GtkWidget *main_tracks_listbox;
    GtkWidget *main_button_update;
    GtkWidget *main_button_open_folder;
    GtkWidget *main_label_name;
    GtkWidget *main_next_button;
    GtkWidget *main_volume_scale;
    GtkWidget *main_button_mode;
    GtkWidget *main_button_one_track;
    GtkWidget *main_button_one_track_loop;
    ButtonState current_button_play_state;
    ButtonState current_button_mod_state;
    ButtonState current_main_button_one_track_loop;
    ButtonState current_main_button_one_track_state;
    GifPersons current_update_person_state;
    MusikData *mData;
    gboolean is_seeking;
    guint timeout_id;
    gboolean is_dragging;
    GtkWidget *change_gif_button;
    GtkWidget *gif_picture;          // виджет для отображения гифки
    GifData *gif_data;              // текущие данные гифки
    int current_gif_index;          // индекс текущей гифки
    char **gif_paths;               // список путей к гифкам
    int gif_count;



}AppData;




#endif