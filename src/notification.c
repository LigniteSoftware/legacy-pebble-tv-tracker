#include <pebble.h>
#include "other.h"
#include "notification.h"

NotificationPushHandler notification_handler;

Notification *notification_create(Window *window, const char *title, const char *content, const GBitmap *background_image){
	Notification *new_notif = malloc(sizeof(Notification));

	new_notif->root_layer = layer_create(GRect(-144, 0, 144, 168));

	new_notif->image_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
	bitmap_layer_set_bitmap(new_notif->image_layer, background_image);
	layer_add_child(new_notif->root_layer, bitmap_layer_get_layer(new_notif->image_layer));

	if(title){
		new_notif->title = text_layer_init(GRect(10, 54, 124, 30), GTextAlignmentCenter, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
		text_layer_set_text(new_notif->title, title);
		text_layer_set_text_color(new_notif->title, GColorBlack);
		layer_add_child(new_notif->root_layer, text_layer_get_layer(new_notif->title));
	}

	new_notif->content = text_layer_init(GRect(15, 80, 119, 168), GTextAlignmentCenter, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	text_layer_set_text(new_notif->content, content);
	text_layer_set_text_color(new_notif->content, GColorBlack);
	layer_add_child(new_notif->root_layer, text_layer_get_layer(new_notif->content));

	if(!title){
		layer_set_frame(text_layer_get_layer(new_notif->content), GRect(10, 64, 124, 148));
		text_layer_set_font(new_notif->content, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	}

	new_notif->destroy_on_finish = false;
	new_notif->root_window = window;
	return new_notif;
}

void notification_destroy(Notification *notification){
	layer_destroy(notification->root_layer);
	bitmap_layer_destroy(notification->image_layer);
	text_layer_destroy(notification->title);
	text_layer_destroy(notification->content);
	if(notification->timer){
		app_timer_cancel(notification->timer);
	}
	free(notification);
}

void notification_finished_callback(void *void_layer){
	Notification *layer = void_layer;
	layer_set_frame(layer->root_layer, GRect(-144, 0, 144, 168));
	layer_remove_from_parent(layer->root_layer);
	if(layer->destroy_on_finish){
		notification_destroy(layer);
	}
	layer->is_live = false;
	if(notification_handler){
		notification_handler(false, layer->id);
	}
}

void notification_force_dismiss(Notification *notification){
	animation_unschedule(notification->end_animation);
	app_timer_cancel(notification->timer);
	GRect middle_frame = GRect(0, 0, 144, 168);
	GRect end_frame = GRect(0, -168, 144, 168);
	notification->end_animation = animate_layer_return(notification->root_layer, &middle_frame, &end_frame, 400, 0);
	notification->timer = app_timer_register(400, notification_finished_callback, notification);
}

void notification_push(Notification* notification, unsigned int length){
	if(!notification){ return; }
	//notification_handler(true);
	layer_add_child(window_get_root_layer(notification->root_window), notification->root_layer);
	notification->is_live = true;
	if(length != 0){
		notification->timer = app_timer_register(length, notification_finished_callback, notification);
	}
	GRect start_frame = layer_get_frame(notification->root_layer);
	GRect middle_frame = GRect(0, 0, 144, 168);
	GRect end_frame = GRect(144, 0, 144, 168);
	animate_layer(notification->root_layer, &start_frame, &middle_frame, 400, 0);
	notification->end_animation = animate_layer_return(notification->root_layer, &middle_frame, &end_frame, 400, length-500);
}

void notification_register_push_handler(NotificationPushHandler handler){
	notification_handler = handler;
}
