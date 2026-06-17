#include <sys/stat.h>
#include <string.h>
#include <gtk/gtk.h>
#include <time.h>
#include<mpg123.h>
#include "audio.h"
#include "dirwork.h"
#include "music_duration.h"
#include "app.h"





static void main_next_button_clicked(GtkButton *btn, gpointer userdata);
static void on_row_activated(GtkListBox *listbox, GtkListBoxRow *row, gpointer userdata);
static void main_play_button_clicked(GtkWidget *btn, gpointer appdata);

static gboolean load_css(void) {
    GtkCssProvider *provider = gtk_css_provider_new();
    gboolean success = FALSE;
    
    // Загружаем CSS файл
    gtk_css_provider_load_from_path(provider, "../ui/style.css");
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
        if(data->current_main_button_one_track_state==OFF && data->current_main_button_one_track_loop==OFF){
            main_next_button_clicked(NULL, data); // переключить на следующий
            return TRUE;
        }else{
            data->is_seeking = TRUE;
            Mix_SetMusicPosition(0.0);
            gtk_range_set_value(GTK_RANGE(data->main_scale), 0.0);
            data->is_seeking = FALSE;
            data->current_button_play_state=DISABLED;
            main_play_button_clicked(NULL, data);
            //Если у нас включен только единоразовый повтор вызываем функцию main_play_button_clicked которая поставит на паузу
            if(data->current_main_button_one_track_state==ON){
                main_play_button_clicked(NULL, data);
            }
            return TRUE;
        }
    }
    return FALSE;
}

static void on_seek_slider_changed(GtkRange *range, gpointer userdata) {
    AppData *data = (AppData*)userdata;
    if (!Mix_PlayingMusic()) return;
    data->is_seeking = TRUE;
    double new_pos = gtk_range_get_value(range);
    Mix_SetMusicPosition(new_pos);
    data->is_seeking = FALSE;
}




static void on_seek_slider_volume_changed(GtkRange *range, gpointer userdata) {
    AppData *data = (AppData*)userdata;
    double new_pos = gtk_range_get_value(range);
    int volume = (int)(new_pos * 128 /100);
    change_audio_volume((int)new_pos);
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
    if(strcmp(data->mData->folderpath,"")==0 || strcmp(data->mData->filename, "")==0) return;
    switch (data->current_button_play_state){
        case DISABLED:
            if (data && data->mData && data->mData->music) {
                Mix_FreeMusic(data->mData->music);
                data->mData->music = NULL;  // всегда обнуляем
            }
            gtk_image_set_from_file(GTK_IMAGE(data->main_play_button_icon), "../ui/pic/pause.png");
            char path[1024];
            char title[2048];
            snprintf(title, sizeof(title), "MP3 Плеер - %s - %s", data->mData->folderpath, data->mData->filename);
            gtk_window_set_title(data->main_window, title);
            snprintf(path, 1024, "%s/%s", data->mData->folderpath,data->mData->filename);
            g_print("\nВот я%s",path);
            data->mData->music = Mix_LoadMUS(path);
            double dur = get_mp3_duration(path);
            data->mData->duration = dur;
            if(dur>0){
                gtk_range_set_range(GTK_RANGE(data->main_scale), 0.0, data->mData->duration);
                gtk_range_set_value(GTK_RANGE(data->main_scale), 0.0);
            }
            gtk_label_set_label(GTK_LABEL(data->main_label_name), data->mData->filename);
            
            play_audio(data->mData->music,0);
            data->current_button_play_state = PLAYED;
            if(data->timeout_id==0){
                data->timeout_id = g_timeout_add(100, update_position_scale, data);

            }
            break;
        case  PAUSED:
            gtk_image_set_from_file(GTK_IMAGE(data->main_play_button_icon), "../ui/pic/pause.png");
            data->current_button_play_state = PLAYED;
            pause_resume_Music();
            break;
        case PLAYED:
            gtk_image_set_from_file(GTK_IMAGE(data->main_play_button_icon), "../ui/pic/play.png");
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
    if (data->mData->current_track_index == -1) return; // ничего не играет
    double pos = Mix_GetMusicPosition(data->mData->music);
    if(pos>3 || data->mData->last_track_index==0){
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
    }else{
        GtkListBox *listbox = GTK_LIST_BOX(data->main_tracks_listbox);
        int prev_index = data->mData->current_track_index - 1;
        GtkListBoxRow *prev_row = gtk_list_box_get_row_at_index(listbox, prev_index);
        if (!prev_row) {
            GtkListBoxRow *next_row = gtk_list_box_get_row_at_index(listbox, data->mData->last_track_index);
            if (!next_row) return;
            on_row_activated(listbox, next_row, data);
            return;
        }
        on_row_activated(listbox, prev_row, data);
    }
}




static void add_track_to_listbox(GtkListBox *listbox, const char *track_name, gpointer userdata){
    AppData* data = (AppData*)userdata;
    GtkWidget *row = gtk_list_box_row_new();
    GtkWidget *label = gtk_label_new(track_name);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0);
    gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), label);
    gtk_list_box_append(listbox, row);
    char path[512];
    snprintf(path, 512, "%s", track_name);
    char *path_copy = g_strdup(path);
    g_object_set_data_full(G_OBJECT(row), "filepath", path_copy, g_free);
    gtk_widget_set_visible(GTK_WIDGET(row), true);
}


static bool is_valid_mp3(const char *path) {
    mpg123_handle *mh = mpg123_new(NULL, NULL);
    if (!mh) return false;

    int err = mpg123_open(mh, path);
    if (err != MPG123_OK) {
        mpg123_delete(mh);
        return false;
    }

    long rate;
    int channels, encoding;
    err = mpg123_getformat(mh, &rate, &channels, &encoding);
    mpg123_close(mh);
    mpg123_delete(mh);

    // Проверяем, что формат корректен (MP3 — это MPG123_ENC_SIGNED_16 или другие)
    return err == MPG123_OK && rate > 0 && channels > 0;
}


static void add_tracks_from_dir(const char *dirname, gpointer userdata){
    AppData* data = (AppData*)userdata;
    DIR *dir = opendir(dirname);
    struct dirent *entry;
    if (dir == NULL){
        perror("opendir"); 
        return;
    }
    int i = 0;
    while(((entry = readdir(dir))!=NULL)){
        if(strcmp(entry->d_name, ".")==0 || strcmp(entry->d_name, "..")==0){
            continue;
        }
        if(entry->d_name[0]!='.'){
            char path[1024];
            snprintf(path,1024,"%s/%s", data->mData->folderpath, entry->d_name);
            if(is_valid_mp3(path)){
                data->mData->last_track_index = i>data->mData->last_track_index?i:data->mData->last_track_index;
                i++;
                add_track_to_listbox(GTK_LIST_BOX(data->main_tracks_listbox), entry->d_name,data);
            }


            // if(strstr(entry->d_name, ".mp3")!=NULL){
            //     data->mData->last_track_index = i>data->mData->last_track_index?i:data->mData->last_track_index;
            //     i++;
            //     add_track_to_listbox(GTK_LIST_BOX(data->main_tracks_listbox), entry->d_name,data);
            // }
        }
    }
    closedir(dir);
}

static void main_button_update_clicked(GtkButton *btn, gpointer userdata){
    AppData *data = (AppData*)userdata;
    gtk_list_box_remove_all(GTK_LIST_BOX(data->main_tracks_listbox));
    data->mData->current_track_index = -1;
    add_tracks_from_dir(data->mData->folderpath, data);
}




static void on_row_activated(GtkListBox *listbox, GtkListBoxRow *row, gpointer userdata){
    AppData *data = (AppData*)userdata;
    if (row == NULL) return;
    gtk_list_box_select_row(listbox, row);
    const char *filepath = g_object_get_data(G_OBJECT(row), "filepath");
    g_print("Тут: %s", filepath);
    if (!filepath) {
        g_printerr("No filepath found for this row\n");
        return;
    }
    data->is_seeking = FALSE;
    if(data->timeout_id>0){
        g_source_remove(data->timeout_id);
        data->timeout_id = 0;
    }
    if (Mix_PlayingMusic()) Mix_HaltMusic();
    data->current_button_play_state=DISABLED;
    // после того как получили row и начали играть
    data->mData->current_track_index = gtk_list_box_row_get_index(row);

    strcpy(data->mData->filename, filepath);
    main_play_button_clicked(data->main_play_button, data);
    g_print("%s", data->mData->filename);
}


static void on_folder_selected(GObject *source, GAsyncResult *result, gpointer userdata) {
    AppData *data = (AppData*)userdata;
    GFile *folder = gtk_file_dialog_select_folder_finish(GTK_FILE_DIALOG(source), result, NULL);
    if (folder) {
        char *folder_path = g_file_get_path(folder);
        if (folder_path) {

            // Очищаем текущий список
            gtk_list_box_remove_all(GTK_LIST_BOX(data->main_tracks_listbox));
            data->mData->last_track_index = 0;
            // Сканируем выбранную папку
            // Меняем заголовок окна (опционально)
            char title[512];
            snprintf(title, sizeof(title), "MP3 Плеер - %s", folder_path);
            gtk_window_set_title(data->main_window, title);
            strncpy(data->mData->folderpath,folder_path,512);
            add_tracks_from_dir(folder_path, data);
            g_print("%s", data->mData->folderpath);
            g_free(folder_path);
        }
        g_object_unref(folder);

    }
    g_object_unref(source);
}
static void on_choose_folder_clicked(GtkButton *button, gpointer userdata) {
    AppData *data = (AppData*)userdata;

    GtkFileDialog *dialog = gtk_file_dialog_new();
    gtk_file_dialog_set_title(dialog, "Выберите папку с музыкой");

    // Асинхронно открываем диалог выбора папки
    gtk_file_dialog_select_folder(dialog, GTK_WINDOW(data->main_window), NULL,
    (GAsyncReadyCallback)on_folder_selected, data);
}





static void main_next_button_clicked(GtkButton *btn, gpointer userdata) {
    AppData *data = (AppData*)userdata;
    if (data->mData->current_track_index == -1) return; // ничего не играет
    GtkListBox *listbox = GTK_LIST_BOX(data->main_tracks_listbox);
    printf("\n%d\n", data->current_button_mod_state);
    switch (data->current_button_mod_state){
    case ON:
        int total_tracks = data->mData->last_track_index + 1;
        if (total_tracks <= 0) return; // нет треков
        int rand_index = rand() % total_tracks;
        GtkListBoxRow *rand_row = gtk_list_box_get_row_at_index(listbox, rand_index);
        if (!rand_row) {
            // на случай, если список изменился – берём первый
            rand_row = gtk_list_box_get_row_at_index(listbox, 0);
            if (!rand_row) return;
        }
        on_row_activated(listbox, rand_row, data);
        break;
    case OFF:
        int next_index = data->mData->current_track_index + 1;
        GtkListBoxRow *next_row = gtk_list_box_get_row_at_index(listbox, next_index);
        if (!next_row) {
            // опционально: зациклить на первый
            next_row = gtk_list_box_get_row_at_index(listbox, 0);
            if (!next_row) return;
        }
        // эмулируем активацию строки (вызываем тот же обработчик, что и двойной клик)
        on_row_activated(listbox, next_row, data);
        break;
    default:
        break;
    }
    
}


static void main_button_mode_clicked(GtkButton *btn, gpointer userdata){
    AppData * data = (AppData*)userdata;

    printf("\n%d\n", data->current_button_mod_state);
    switch (data->current_button_mod_state){
    case ON:
        data->current_button_mod_state = OFF;
        gtk_widget_remove_css_class(data->main_button_mode, "button_state_on");
        break;
        case OFF:
        data->current_button_mod_state = ON;
        gtk_widget_add_css_class(data->main_button_mode, "button_state_on");
        break;
    default:
        break;
    }
}

static void main_button_one_track_clicked(GtkButton *btn, gpointer userdata){
    AppData *data = (AppData*)userdata;
    switch (data->current_main_button_one_track_state){
    case OFF:
        data->current_main_button_one_track_state = ON;
        gtk_widget_add_css_class(data->main_button_one_track, "button_state_on");
        break;
    case ON:
        data->current_main_button_one_track_state = OFF;
        gtk_widget_remove_css_class(data->main_button_one_track, "button_state_on");
        break;
    default:
        break;
    }
}

static void main_button_one_track_loop_clicked(GtkButton *btn, gpointer userdata){
    AppData *data = (AppData*)userdata;
    switch (data->current_main_button_one_track_loop){
    case OFF:
        data->current_main_button_one_track_loop = ON;
        gtk_widget_add_css_class(data->main_button_one_track_loop, "button_state_on");
        break;
    case ON:
        data->current_main_button_one_track_loop = OFF;
        gtk_widget_remove_css_class(data->main_button_one_track_loop, "button_state_on");
        break;
    default:
        break;
    }
}



static void on_window_destroy(GtkWindow *win, gpointer userdata) {
    AppData *data = (AppData*)userdata;
    
    // Останавливаем таймер, если активен
    if (data->timeout_id) {
        g_source_remove(data->timeout_id);
        data->timeout_id = 0;
    }
    
    // Освобождаем аудиоресурсы
    if (data->mData) {
        if (data->mData->music) {
            Mix_FreeMusic(data->mData->music);
            data->mData->music = NULL;
        }
        free(data->mData);
    }
    
    // Освобождаем основную структуру
    free(data);
}




static void app_activate(GtkApplication *app){
    srand(time(NULL));
    load_css();
    GtkBuilder *builder = gtk_builder_new_from_file("../ui/builder.ui");
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
    strcpy(mData->filename,"");
    strcpy(mData->folderpath,"");
    data->current_button_play_state=DISABLED;
    data->current_main_button_one_track_loop = OFF;
    data->current_button_mod_state = OFF; 
    data->current_main_button_one_track_state=OFF;
    data->main_window = GTK_WINDOW(window);
    data->main_play_button = GTK_WIDGET(gtk_builder_get_object(builder, "main_play_button"));
    data->main_play_button_icon = GTK_WIDGET(gtk_builder_get_object(builder, "main_play_button_icon"));
    data->main_prev_button = GTK_WIDGET(gtk_builder_get_object(builder,"main_prev_button"));
    data->main_scale = GTK_WIDGET(gtk_builder_get_object(builder,"main_scale"));
    data->main_label_time = GTK_WIDGET(gtk_builder_get_object(builder, "main_label_time"));
    data->main_tracks_listbox = GTK_WIDGET(gtk_builder_get_object(builder,"main_tracks_listbox"));
    data->main_button_update = GTK_WIDGET(gtk_builder_get_object(builder,"main_button_update"));
    data->main_button_open_folder = GTK_WIDGET(gtk_builder_get_object(builder, "main_button_open_folder"));
    data->main_label_name = GTK_WIDGET(gtk_builder_get_object(builder, "main_label_name"));
    data->main_next_button = GTK_WIDGET(gtk_builder_get_object(builder,"main_next_button"));
    data->main_volume_scale = GTK_WIDGET(gtk_builder_get_object(builder,"main_volume_scale"));
    data->main_button_mode = GTK_WIDGET(gtk_builder_get_object(builder, "main_button_mode"));
    data->main_button_one_track = GTK_WIDGET(gtk_builder_get_object(builder, "main_button_one_track"));
    data->main_button_one_track_loop = GTK_WIDGET(gtk_builder_get_object(builder, "main_button_one_track_loop"));


    //Удаляем билдер
    g_object_unref(builder);


    data->mData = mData;
    data->mData->music = NULL;
    data->mData->current_track_index = -1;
    data->is_seeking = FALSE;
    data->timeout_id = 0;
    data->mData->last_track_index = 0;


    //Подключаем обработчик закрытия окная
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), data);


    g_signal_connect(data->main_button_update, "clicked", G_CALLBACK(main_button_update_clicked), data);
    g_signal_connect(data->main_play_button, "clicked", G_CALLBACK(main_play_button_clicked), data);
    g_signal_connect(data->main_prev_button, "clicked", G_CALLBACK(main_prev_button_clicked), data);
    g_signal_connect(data->main_scale, "value-changed", G_CALLBACK(on_seek_slider_changed), data);
    g_signal_connect(data->main_tracks_listbox, "row-activated", G_CALLBACK(on_row_activated), data);
    g_signal_connect(data->main_volume_scale, "value-changed", G_CALLBACK(on_seek_slider_volume_changed), data);
    g_signal_connect(data->main_button_open_folder, "clicked", G_CALLBACK(on_choose_folder_clicked), data);
    g_signal_connect(data->main_next_button, "clicked", G_CALLBACK(main_next_button_clicked), data);
    g_signal_connect(data->main_button_mode, "clicked", G_CALLBACK(main_button_mode_clicked), data);
    g_signal_connect(data->main_button_one_track, "clicked", G_CALLBACK(main_button_one_track_clicked), data);
    g_signal_connect(data->main_button_one_track_loop, "clicked", G_CALLBACK(main_button_one_track_loop_clicked), data);
    gtk_list_box_set_activate_on_single_click(GTK_LIST_BOX(data->main_tracks_listbox),FALSE);
    gtk_window_set_application(GTK_WINDOW(window), app);
    gtk_window_present(GTK_WINDOW(window));

}

int main (int argc, char **argv){
    init_audio();
    GtkApplication *app = gtk_application_new ("com.metanit", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(app_activate), NULL);
    int status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref(app);
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
 
    return status;
}