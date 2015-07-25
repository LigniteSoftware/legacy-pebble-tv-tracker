#pragma once
#define MAX_AMOUNT_OF_SHOWS 90
#include "user_data.h"

void new_show_set_channel(Channel channel);
void new_show_layer_add_show(LargeShow show);
void new_shows_layer_init();
void new_shows_layer_deinit();
Window* new_shows_layer_get_window();
