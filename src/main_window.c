#include <pebble.h>
#include <data_framework.h>
#include "main_window.h"
#include "user_data.h"

Window *main_window;

void main_window_load(Window *window){
	if(!user_data_is_logged_in()){

	}
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
