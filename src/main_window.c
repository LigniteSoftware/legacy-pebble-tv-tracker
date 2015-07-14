#include <pebble.h>
#include <lignite.h>
#include "main_window.h"

Window *main_window;

void main_window_update_settings(Settings new_settings){

}

void main_window_load(Window *window){

}

void main_window_unload(Window *window){

}

void main_window_init(){
	main_window = window_create();
	window_set_background_color(main_window, GColorBlack);
	window_set_window_handlers(main_window, (WindowHandlers){
		.load = main_window_load,
		.unload = main_window_unload
	});
}

Window *main_window_get_window(){
	return main_window;
}

void main_window_destroy(){
	window_destroy(main_window);
}
