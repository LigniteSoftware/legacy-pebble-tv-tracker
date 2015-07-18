#pragma once
#include "user_data.h"

typedef LargeShow*(*NextShowCallback)(int index);

void show_detail_register_next_show_callback(NextShowCallback callback);
void show_detail_set_large_show(LargeShow show);
void show_detail_set_show(Show show);
void show_detail_layer_init();
void show_detail_layer_deinit();
Window *show_detail_layer_get_window();
