#pragma once
#include "user_data.h"
#include "data_framework.h"

typedef struct TextLayerUpdate {
    TextLayer *layer;
    char *new_text;
} TextLayerUpdate;

typedef LargeShow*(*NextShowCallback)(int index);

void show_detail_register_next_show_callback(NextShowCallback callback);
void show_detail_set_large_show(LargeShow show);
void show_detail_set_show(Show show);
void show_detail_layer_init();
void show_detail_layer_deinit();
Window *show_detail_layer_get_window();
void show_detail_status_callback(ActionStatus status);
void show_detail_set_index(int index);
void show_detail_set_min_max(int min, int max);
