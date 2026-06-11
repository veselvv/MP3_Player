#include <sys/stat.h>
#include <string.h>
#include <gtk/gtk.h>
#include "../new_file.h"
#include "../dirwork.h"
#include "../music_duration.h"


typedef enum{
    PAUSED,
    PLAYED,
    DISABLED
}MainPlayButtonState;


typedef struct{
    Mix_Music *music;
    double duration;
}MusikData;


typedef struct{
    GtkWindow *main_window;
    GtkWidget *main_play_button;
    GtkWidget *main_play_button_icon;
    GtkWidget *main_prev_button;
    GtkWidget *main_scale;
    MainPlayButtonState current_button_play_state;
    MusikData *mData;
    guint timeout_id;
    gboolean is_seeking;
    GtkWidget *main_label_time;
}AppData;



static gboolean load_css(void) {
    GtkCssProvider *provider = gtk_css_provider_new();
    gboolean success = FALSE;
    
    // Загружаем CSS файл
    gtk_css_provider_load_from_path(provider, "style.css");
    gtk_style_context_add_provider_for_display(
            gdk_display_get_default(),
            GTK_STYLE_PROVIDER(provider),
            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
        );
        success = TRUE;
        g_print("CSS loaded successfully\n");
    
    g_object_unref(provider);
    return success;
}

static gboolean update_position_scale(gpointer userdata){
    AppData *data = (AppData*)userdata;
    if(data->is_seeking) return TRUE;

    if(Mix_PlayingMusic()){
        double pos = Mix_GetMusicPosition(data->mData->music);
        if(pos>=0 && data->mData->duration > 0){
            gtk_range_set_value(GTK_RANGE(data->main_scale), pos);
            char string[100];
            sprintf(string,"%02d:%02d/%02d:%02d", (int)pos/60, (int)pos%60, (int)data->mData->duration/60, (int)data->mData->duration%60);
            gtk_label_set_label(GTK_LABEL(data->main_label_time), string);
        }
        return TRUE;
    }else{
        if(data->timeout_id){
            g_source_remove(data->timeout_id);
            data->timeout_id = 0;
            
        }
        return FALSE;
    }
}



static void on_seek_slider_changed(GtkWidget *raange,gpointer userdata){
    AppData *data = (AppData*)userdata;
    if(!Mix_PlayingMusic()) return;
    data->is_seeking = TRUE;
    double new_pos = gtk_range_get_value(GTK_RANGE(data->main_scale));
    Mix_SetMusicPosition(new_pos);
    data->is_seeking = FALSE;
}


static void from_the_top_slider(gpointer userdata){
    AppData *data = (AppData*)userdata;
    if(!Mix_PlayingMusic()) return;
    data->is_seeking = TRUE;
    Mix_SetMusicPosition(0.0);
    gtk_range_set_value(GTK_RANGE(data->main_scale), 0.0);
    data->is_seeking = FALSE;
}

static void main_play_button_clicked(GtkWidget *btn, gpointer appdata){
    AppData *data = (AppData*)appdata;
    switch (data->current_button_play_state){
        case DISABLED:
            gtk_image_set_from_file(GTK_IMAGE(data->main_play_button_icon), "pic/pause.png");
            const char *filename = "/home/vvv/Desktop/projects/player/testsdl2/tracks/ew.mp3";
            data->mData->music = Mix_LoadMUS(filename);
            double dur = get_mp3_duration(filename);
            data->mData->duration = dur;
            if(dur>0){
                gtk_range_set_range(GTK_RANGE(data->main_scale), 0.0, data->mData->duration);
                gtk_range_set_value(GTK_RANGE(data->main_scale), 0.0);
            }
            play_audio(data->mData->music,0);
            data->current_button_play_state = PLAYED;
            if(data->timeout_id==0){
                data->timeout_id = g_timeout_add(100, update_position_scale, data);

            }
            break;
        case  PAUSED:
            gtk_image_set_from_file(GTK_IMAGE(data->main_play_button_icon), "pic/pause.png");
            data->current_button_play_state = PLAYED;
            pause_resume_Music();
            break;
        case PLAYED:
            gtk_image_set_from_file(GTK_IMAGE(data->main_play_button_icon), "pic/play.png");
            data->current_button_play_state = PAUSED;
            pause_resume_Music();
            break;
        default:
            printf("Не удалось сменить режим\n");
            break;
    }
}



static void main_prev_button_clicked(GtkWidget *btn, gpointer userdata){
    AppData *data = (AppData*)userdata;
    switch (data->current_button_play_state){
    case  PLAYED:
        if(Mix_PlayingMusic()){
            play_from_start();
        }else{
            play_audio(data->mData->music, 0);
            from_the_top_slider(data);
            update_position_scale(data);
            if(data->timeout_id==0){
                data->timeout_id = g_timeout_add(100, update_position_scale, data);

            }
        }
        break;
    case PAUSED:
         if(Mix_PlayingMusic()){
            play_from_start();
        }else{
            play_audio(data->mData->music, 0);
            from_the_top_slider(data);
            update_position_scale(data);
            if(data->timeout_id==0){
                data->timeout_id = g_timeout_add(100, update_position_scale, data);

            }
        }
        break;
    default:
        break;
    }
}

static void app_activate(GtkApplication *app){
    load_css();
    GtkBuilder *builder = gtk_builder_new_from_file("builder.ui");
    GObject *window = gtk_builder_get_object(builder, "window");
    AppData *data = malloc(sizeof(AppData));
    if(!data){
        perror("Error:");
        exit(1);
    }
    MusikData *mData = malloc(sizeof(MusikData));
     if(!mData){
        perror("Error:");
        exit(1);
    }
    data->current_button_play_state=DISABLED;
    data->main_window = GTK_WINDOW(window);
    data->main_play_button = GTK_WIDGET(gtk_builder_get_object(builder, "main_play_button"));
    data->main_play_button_icon = GTK_WIDGET(gtk_builder_get_object(builder, "main_play_button_icon"));
    data->main_prev_button = GTK_WIDGET(gtk_builder_get_object(builder,"main_prev_button"));
    data->main_scale = GTK_WIDGET(gtk_builder_get_object(builder,"main_scale"));
    data->mData = mData;
    data->main_label_time = GTK_WIDGET(gtk_builder_get_object(builder, "main_label_time"));
    g_signal_connect(data->main_play_button, "clicked", G_CALLBACK(main_play_button_clicked), data);
    g_signal_connect(data->main_prev_button, "clicked", G_CALLBACK(main_prev_button_clicked), data);
    g_signal_connect(data->main_scale, "value-changed", G_CALLBACK(on_seek_slider_changed), data);

    gtk_window_set_application(GTK_WINDOW(window), app);
    gtk_window_present(GTK_WINDOW(window));
}

int main (int argc, char **argv){
    init_audio();
    GtkApplication *app = gtk_application_new ("com.metanit", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(app_activate), NULL);
    int status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref(app);
 
    return status;
}