#include <pebble.h>
#include <lignite.h>
#include "main_window.h"
#include "graphics.h"

Window *main_window;
Settings settings;
TextLayer *time_layer;
Layer *main_graphics_layer, *window_layer;
Notification *disconnect_notification, *reconnect_notification;

void main_tick_handler(struct tm *t, TimeUnits units){
	static char time_buffer[] = "00:00";
	if(clock_is_24h_style()){
		strftime(time_buffer, sizeof(time_buffer), "%I:%M", t);
	}
	else{
		strftime(time_buffer, sizeof(time_buffer), "%H:%M", t);
	}
	text_layer_set_text(time_layer, time_buffer);

	graphics_set_time(t);

	if(!trial_is_verified()){
		trial_tick();
		if(trial_time_left() < 0 && !trial_get_trial_notification()->is_live){
			notification_push(trial_get_trial_notification(), 0);
			trial_save();
		}
	}
}

void bluetooth_handler(bool connected){
	if(connected && settings.btrealert){
		notification_push(reconnect_notification, 5000);
		vibes_double_pulse();
	}
	else if(!connected && settings.btdisalert){
		notification_push(disconnect_notification, 5000);
		vibes_long_pulse();
	}
}

void notification_push_handler(bool pushed, uint8_t id){
	if(pushed){
		layer_remove_from_parent(window_layer);
	}
	else{
		if(disconnect_notification->is_live || reconnect_notification->is_live){
			return;
		}
		layer_add_child(window_get_root_layer(main_window), window_layer);
	}
}

void main_window_settings_callback(Settings new_settings){
	settings = new_settings;

	text_layer_set_text_color(time_layer, settings.time_colour);

	struct tm *t;
  	time_t temp;
  	temp = time(NULL);
  	t = localtime(&temp);
  	main_tick_handler(t, MINUTE_UNIT);
}

void main_graphics_proc(Layer *layer, GContext *ctx){
	graphics_context_set_fill_color(ctx, settings.circle_colour);
	graphics_context_set_stroke_color(ctx, GColorBlack);

	graphics_fill_circle(ctx, MAIN_CIRCLE_LOCATION, MAIN_CIRCLE_RADIUS);
	for(int i = 0; i < 6; i++){
		graphics_draw_rect(ctx, GRect(i, i, 144-(i*2), 168-(i*2)));
	}
}

void main_window_load(Window *window){
	window_layer = layer_create(GRect(0, 0, 144, 168));
	layer_add_child(window_get_root_layer(window), window_layer);

	graphics_init(window_layer);

	GFont *font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);

	main_graphics_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(main_graphics_layer, main_graphics_proc);
	layer_add_child(window_layer, main_graphics_layer);

	time_layer = text_layer_init(GRect(0, 68, 144, 168), GTextAlignmentCenter, font);
	text_layer_set_text(time_layer, "13:00");
	layer_add_child(window_layer, text_layer_get_layer(time_layer));

	disconnect_notification = notification_create(window, "Whoops!", "Your Bluetooth connection has been lost, sorry.", gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BT_DISCONNECTED));
    reconnect_notification = notification_create(window, "Woohoo!", "You are now connected again!", gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BT_RECONNECTED));
    notification_register_push_handler(notification_push_handler);

	data_framework_register_settings_callback(main_window_settings_callback, SETTINGS_CALLBACK_MAIN_WINDOW);

	trial_create(window);

	settings = data_framework_get_settings();
	main_window_settings_callback(settings);
}

void main_window_unload(Window *window){
	graphics_deinit();
}

void main_window_init(){
	main_window = window_create();
	window_set_background_color(main_window, GColorBlack);
	window_set_window_handlers(main_window, (WindowHandlers){
		.load = main_window_load,
		.unload = main_window_unload
	});
	tick_timer_service_subscribe(MINUTE_UNIT, main_tick_handler);
	bluetooth_connection_service_subscribe(bluetooth_handler);
}

Window *main_window_get_window(){
	return main_window;
}

void main_window_destroy(){
	window_destroy(main_window);
}
