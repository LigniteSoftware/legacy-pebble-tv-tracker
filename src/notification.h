#pragma once
#include <pebble.h>

typedef struct Notification {
	Layer *root_layer;
	Window *root_window;
	TextLayer *title;
	TextLayer *content;
	BitmapLayer *image_layer;
	AppTimer *timer;
	Animation *end_animation;
	uint8_t id;
	bool destroy_on_finish;
	bool is_live;
} Notification;

typedef void (*NotificationPushHandler)(bool pushed, uint8_t id);

Notification *notification_create(Window *window, const char *title, const char *content, const GBitmap *background_image);
void notification_destroy(Notification *notification);
void notification_push(Notification* notification, unsigned int length);
void notification_force_dismiss(Notification *notification);
void notification_register_push_handler(NotificationPushHandler handler);
