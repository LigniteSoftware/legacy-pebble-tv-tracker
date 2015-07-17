#include <pebble.h>
#include "show_detail_layer.h"
#include "data_framework.h"

Window *show_detail_window;
LargeShow show_detail_show;

void show_detail_set_show(LargeShow show){
	show_detail_show = show;
}

void show_detail_window_load(Window *window){

}

void show_detail_window_unload(Window *window){

}

void show_detail_layer_init(){
	show_detail_window = window_create();
	window_set_window_handlers(show_detail_window, (WindowHandlers){
		.load = show_detail_window_load,
		.unload = show_detail_window_unload
	});
}

void show_detail_layer_deinit(){
	window_destroy(show_detail_window);
}

Window *show_detail_layer_get_window(){
	return show_detail_window;
}
