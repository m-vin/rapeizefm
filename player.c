#include <gtk/gtk.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>
#include <string.h>
#include <id3v2lib.h>
#include "player.h"

char song[100];
Mix_Music *music;

int channel_num = 2;
int previous_volume = 0;
GtkWidget *mono_widget;
GtkWidget *stereo_widget;

GtkToolItem *about_button;

GtkToolItem *import_music_button;

GtkToolItem *play_button;
GtkToolItem *pause_button;
GtkToolItem *stop_button;

GtkToolItem *mono_button;
GtkToolItem *stereo_button;

GtkWidget *vbox;

GtkWidget *l_artist;
GtkWidget *l_album;
GtkWidget *l_title;

int destroy(){
	gtk_main_quit();
	Mix_Quit();
	return 0;
}

void swap_tool_button(GtkWidget *widget_to_hide, GtkWidget *widget_to_show){
	gtk_widget_hide(GTK_WIDGET(widget_to_hide));
	gtk_widget_show(GTK_WIDGET(widget_to_show));
}

void play_music(){
	swap_tool_button(GTK_WIDGET(play_button), GTK_WIDGET(pause_button));
	gtk_widget_set_sensitive(GTK_WIDGET(stop_button), TRUE);
	if (!Mix_PausedMusic()) {
		Mix_PlayMusic(music, 1);
	}
	else {
		Mix_ResumeMusic();
	}
	check_music();
}

void pause_music(){
	swap_tool_button(GTK_WIDGET(pause_button), GTK_WIDGET(play_button));
	Mix_PauseMusic();
	check_music();
}

void stop_music(){
	if (Mix_PausedMusic()){
		previous_volume = Mix_VolumeMusic(0);
		printf("%d\n", previous_volume);
		Mix_ResumeMusic();
	}
	swap_tool_button(GTK_WIDGET(pause_button), GTK_WIDGET(play_button));
	Mix_HaltMusic();
	gtk_widget_set_sensitive(GTK_WIDGET(stop_button), FALSE);
	Mix_VolumeMusic(previous_volume);
	set_audio();
}

void check_music(){
	printf("music is%s paused\n", Mix_PausedMusic()?"":" not");
}

void change_to_mono(){
	stop_music();
	gtk_widget_hide(GTK_WIDGET(mono_button));
	gtk_widget_show(GTK_WIDGET(stereo_button));
	channel_num = 1;
	printf("mono\n");
}

void change_to_stereo(){
	stop_music();
	gtk_widget_hide(GTK_WIDGET(stereo_button));
	gtk_widget_show(GTK_WIDGET(mono_button));
	channel_num = 2;
	printf("stereo\n");
}

void import_music(){
	GtkWidget *dialog;
	dialog = gtk_file_chooser_dialog_new("Choose music", NULL, GTK_FILE_CHOOSER_ACTION_OPEN,
						GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN,
						GTK_RESPONSE_ACCEPT, NULL);
	if (gtk_dialog_run (GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT){
		char *filename;
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		strcpy(song, filename);
	}
	gtk_widget_destroy(dialog);
	if (strcmp(song, "") == 0){
		gtk_widget_set_sensitive(GTK_WIDGET(play_button), FALSE);
	}
	else{
		rapeizefm_audio_init();
		gtk_widget_set_sensitive(GTK_WIDGET(play_button), TRUE);
	}
}

const char* get_artist(){
	static gchar *artist;
	ID3v2_tag* tag = load_tag(song);
	if (tag == NULL){
		tag = new_tag();
	}
        ID3v2_frame* artist_frame = tag_get_artist(tag);
        ID3v2_frame_text_content* artist_content = parse_text_frame_content(artist_frame);
        artist = g_locale_to_utf8(artist_content->data, -1, 0, 0, NULL);

        g_print("%s\n", artist);

	return artist;
}

const char* get_album(){
        static gchar *album;
        ID3v2_tag* tag = load_tag(song);
        if (tag == NULL){
                tag = new_tag();
        }

        ID3v2_frame* album_frame = tag_get_album(tag);
        ID3v2_frame_text_content* album_content = parse_text_frame_content(album_frame);
        album = g_locale_to_utf8(album_content->data, -1, 0, 0, NULL);

        g_print("%s\n", album);

        return album;
}

const char* get_title(){
        static gchar *title;
        ID3v2_tag* tag = load_tag(song);
        if (tag == NULL){
                tag = new_tag();
        }
        ID3v2_frame* title_frame = tag_get_title(tag);
        ID3v2_frame_text_content* title_content = parse_text_frame_content(title_frame);
        title = g_locale_to_utf8(title_content->data, -1, 0, 0, NULL);

        g_print("%s\n", title);
        return title;
}

void rapeizefm_audio_init(){
        int result = 0;
        int flags = MIX_INIT_MP3;

	gtk_label_set_text(GTK_LABEL(l_artist), get_artist());
	gtk_label_set_text(GTK_LABEL(l_album), get_album());
        gtk_label_set_text(GTK_LABEL(l_title), get_title());

        if (SDL_Init(SDL_INIT_AUDIO) < 0){
                printf("Failed to init SDL\n");
                exit(1);
        }

        if (flags != (result = Mix_Init(flags))){
                printf("Could not initialize mixer (result: %d).\n", result);
                printf("Mix_Init: %s\n", Mix_GetError());
                exit(1);
        }
        set_audio();
}

void set_audio(){
        Mix_OpenAudio(44100, AUDIO_S16SYS, channel_num, 1024);

        music = Mix_LoadMUS(song);
        if (!music){
                printf("Mix_LoadMUS(\"%s\"): %s\n", song, Mix_GetError());
        }
}

static void about(GtkWidget *widget, gpointer data){

        GtkWidget *about;
        GdkPixbuf *icon = gdk_pixbuf_new_from_file("icon.png", NULL);

        about = gtk_about_dialog_new();
        gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(about), "rapeizefm media player");
        gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about), "(c) mavingrado");
        gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(about), "media player written by mavingrado");
        gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(about), "http://www.github.com/m-vin");
        gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(about), icon);
        g_object_unref(icon), icon = NULL;
        gtk_dialog_run(GTK_DIALOG(about));
        gtk_widget_destroy(about);
}

GdkPixbuf *create_pixbuf(const gchar* filename){
	GdkPixbuf *pixbuf;
	GError *error = NULL;
	pixbuf = gdk_pixbuf_new_from_file(filename, &error);

	if(!pixbuf){
		fprintf(stderr, "%s\n", error->message);
		g_error_free(error);
	}
	return pixbuf;
}

int main(int argc, char* argv[]){
	GtkWidget *window;
	GtkWidget *toolbar;
	GdkPixbuf *icon;

	printf("%s\n", song);

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "rapeize media player");
	gtk_window_set_default_size(GTK_WINDOW(window), 280, 150);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_widget_show(window);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(window), vbox);
	gtk_widget_show(GTK_WIDGET(vbox));

	toolbar = gtk_toolbar_new();
	gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
	gtk_widget_show(GTK_WIDGET(toolbar));

	play_button = gtk_tool_button_new_from_stock(GTK_STOCK_MEDIA_PLAY);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), play_button, 0);
	gtk_widget_show(GTK_WIDGET(play_button));
	gtk_widget_set_sensitive(GTK_WIDGET(play_button), FALSE);

	pause_button = gtk_tool_button_new_from_stock(GTK_STOCK_MEDIA_PAUSE);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), pause_button, 0);

	stop_button = gtk_tool_button_new_from_stock(GTK_STOCK_MEDIA_STOP);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), stop_button, -1);
	gtk_widget_show(GTK_WIDGET(stop_button));
	gtk_widget_set_sensitive(GTK_WIDGET(stop_button), FALSE);

	mono_button = gtk_tool_button_new(mono_widget, "change to mono");
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), mono_button, -2);
	gtk_widget_show(GTK_WIDGET(mono_button));

	stereo_button = gtk_tool_button_new(stereo_widget, "change to stereo");
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), stereo_button, -2);

	import_music_button = gtk_tool_button_new_from_stock(GTK_STOCK_OPEN);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), import_music_button, -3);
	gtk_widget_show(GTK_WIDGET(import_music_button));

        about_button = gtk_tool_button_new_from_stock(GTK_STOCK_DIALOG_QUESTION);
        gtk_toolbar_insert(GTK_TOOLBAR(toolbar), about_button, -4);
        gtk_widget_show(GTK_WIDGET(about_button));

	gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);

	l_artist = gtk_label_new("");
	gtk_box_pack_start(GTK_BOX(vbox), l_artist, TRUE, TRUE, 0);
	gtk_widget_show(GTK_WIDGET(l_artist));

	l_album = gtk_label_new("");
	gtk_box_pack_start(GTK_BOX(vbox), l_album, TRUE, TRUE, 0);
	gtk_widget_show(GTK_WIDGET(l_album));

	l_title = gtk_label_new("");
	gtk_box_pack_start(GTK_BOX(vbox), l_title, TRUE, TRUE, 0);
	gtk_widget_show(GTK_WIDGET(l_title));

	icon = create_pixbuf("icon.png");
	gtk_window_set_icon(GTK_WINDOW(window), icon);

	g_signal_connect(G_OBJECT(about_button), "clicked", G_CALLBACK(about), GTK_WINDOW(window));
	g_signal_connect(G_OBJECT(import_music_button), "clicked", G_CALLBACK(import_music), GTK_WINDOW(window));
	g_signal_connect(G_OBJECT(play_button), "clicked", G_CALLBACK(play_music), GTK_WINDOW(window));
	g_signal_connect(G_OBJECT(pause_button), "clicked", G_CALLBACK(pause_music), GTK_WINDOW(window));
	g_signal_connect(G_OBJECT(stop_button), "clicked", G_CALLBACK(stop_music), GTK_WINDOW(window));
	g_signal_connect(G_OBJECT(mono_button), "clicked", G_CALLBACK(change_to_mono), GTK_WINDOW(window));
	g_signal_connect(G_OBJECT(stereo_button), "clicked", G_CALLBACK(change_to_stereo), GTK_WINDOW(window));
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

	g_object_unref(icon);

	gtk_main();

	return 0;
}
