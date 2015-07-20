#pragma once
#include "user_data.h"

void shows_layer_init();
void shows_layer_deinit();
Window* shows_layer_get_window();
void shows_layer_add_show(Show show);
bool shows_layer_is_full();
void shows_layer_delete(int index);
