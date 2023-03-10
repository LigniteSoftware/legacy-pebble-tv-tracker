#include <pebble.h>
#include <data_framework.h>
#include "main_window.h"
#include "user_data.h"
#include "shows_layer.h"
#include "channels_layer.h"
#include "show_detail_layer.h"

Window *main_window;
TextLayer *error_layer;
bool bluetooth_good = false;

void update_window(){
	if(bluetooth_good){
		window_stack_pop_all(true);
		window_stack_push(shows_layer_get_window(), true);
	}
	else{
		if(window_stack_get_top_window() != main_window){
			window_stack_remove(window_stack_get_top_window(), true);
			window_stack_push(main_window, true);
		}
	}
}

void bluetooth_handler(bool connected){
	if(!connected){
		text_layer_set_text(error_layer, "Uh oh\n\nBluetooth is not connected. I need Bluetooth to function, sorry!");
	}
	bluetooth_good = connected;
	update_window();
}

void main_window_load(Window *window){
	error_layer = text_layer_create(GRect(10, 0, 124, 168));
	text_layer_set_font(error_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text_color(error_layer, GColorWhite);
	text_layer_set_text_alignment(error_layer, GTextAlignmentCenter);
	text_layer_set_background_color(error_layer, GColorClear);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(error_layer));

	bluetooth_handler(bluetooth_connection_service_peek());
}

void main_window_unload(Window *window){
	text_layer_destroy(error_layer);
}

void main_window_init(){
	main_window = window_create();
	window_set_background_color(main_window, GColorBlack);
	window_set_window_handlers(main_window, (WindowHandlers){
		.load = main_window_load,
		.unload = main_window_unload
	});
	bluetooth_connection_service_subscribe(bluetooth_handler);
}

Window *main_window_get_window(){
	return main_window;
}

void main_window_destroy(){
	window_destroy(main_window);
}
