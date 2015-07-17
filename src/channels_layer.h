#pragma once
#define AMOUNT_OF_CHANNELS 120
#include "user_data.h"

void channels_layer_add_channel(Channel channel);
void channels_layer_init();
void channels_layer_deinit();
Window* channels_layer_get_window();
