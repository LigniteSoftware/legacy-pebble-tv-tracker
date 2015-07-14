#pragma once
#define MAIN_CIRCLE_LOCATION GPoint(72, 84)
#define MAIN_CIRCLE_RADIUS 30

void main_window_init();
Window *main_window_get_window();
void main_window_destroy();
void main_window_update_settings(Settings new_settings);
