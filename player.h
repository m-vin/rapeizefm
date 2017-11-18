#ifndef PLAYER_H_
#define PLAYER_H_

int destroy();
void swap_tool_button(GtkWidget*, GtkWidget*);
void play_music();
void pause_music();
void check_music();
void change_to_mono();
void change_to_stereo();
const char* get_artist();
const char* get_album();
const char* get_title();
void rapeizefm_audio_init();
void set_audio();
static void about();

#endif
