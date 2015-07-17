#include <pebble.h>
#include "show_detail_layer.h"
#include "data_framework.h"

static const LargeShow large_show_blank;

Window *show_detail_window;
LargeShow show_detail_show;
GBitmap *subscribe_icon, *unsubscribe_icon, *up_icon, *down_icon;
ActionBarLayer *show_detail_action_bar;
TextLayer *show_name_layer, *show_channel_layer, *show_start_layer, *show_end_layer, *show_new_layer;

bool show_detail_is_already_subscribed(){
	return (show_detail_show.start == 0);
}

void show_detail_set_large_show(LargeShow show){
	APP_LOG(APP_LOG_LEVEL_INFO, "Got show with name: %s", show.base_show.name[0]);
	memcpy(&show_detail_show, &show, sizeof(LargeShow));
}

void show_detail_set_show(Show show){
	memcpy(&show_detail_show.base_show, &show, sizeof(Show));
}

void show_detail_next_show(ClickRecognizerRef referee, void *ctx){

}

void show_detail_previous_show(ClickRecognizerRef referee, void *ctx){

}

void show_detail_sub_action(ClickRecognizerRef referee, void *ctx){

}

void show_detail_click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_DOWN, show_detail_next_show);
	window_single_click_subscribe(BUTTON_ID_SELECT, show_detail_sub_action);
	window_single_click_subscribe(BUTTON_ID_UP, show_detail_previous_show);
}

void show_detail_window_load(Window *window){
	Layer *window_layer = window_get_root_layer(window);

	show_detail_action_bar = action_bar_layer_create();
	action_bar_layer_add_to_window(show_detail_action_bar, window);
	action_bar_layer_set_click_config_provider(show_detail_action_bar, show_detail_click_config_provider);

	up_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_UP_ICON);
	down_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DOWN_ICON);
	subscribe_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SUB_ICON);
	unsubscribe_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_UNSUB_ICON);

	action_bar_layer_set_icon_animated(show_detail_action_bar, BUTTON_ID_UP, up_icon, true);
	action_bar_layer_set_icon_animated(show_detail_action_bar, BUTTON_ID_SELECT, show_detail_is_already_subscribed() ? unsubscribe_icon : subscribe_icon, true);
	action_bar_layer_set_icon_animated(show_detail_action_bar, BUTTON_ID_DOWN, down_icon, true);

	show_name_layer = text_layer_create(GRect(10, 0, 144-ACTION_BAR_WIDTH-20, 168));
	text_layer_set_text_alignment(show_name_layer, GTextAlignmentCenter);
	text_layer_set_text(show_name_layer, show_detail_show.base_show.name[0]);
	text_layer_set_font(show_name_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
	text_layer_set_background_color(show_name_layer, GColorClear);
	layer_add_child(window_layer, text_layer_get_layer(show_name_layer));

	show_channel_layer = text_layer_create(GRect(0, 55, 144-ACTION_BAR_WIDTH, 168));
	text_layer_set_text_alignment(show_channel_layer, GTextAlignmentCenter);
	text_layer_set_text(show_channel_layer, show_detail_show.base_show.channel.name[0]);
	text_layer_set_font(show_channel_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	text_layer_set_background_color(show_channel_layer, GColorClear);
	layer_add_child(window_layer, text_layer_get_layer(show_channel_layer));
}

void show_detail_window_unload(Window *window){
	action_bar_layer_destroy(show_detail_action_bar);

	text_layer_destroy(show_name_layer);
	text_layer_destroy(show_channel_layer);

	gbitmap_destroy(subscribe_icon);
	gbitmap_destroy(unsubscribe_icon);
	gbitmap_destroy(up_icon);
	gbitmap_destroy(down_icon);

	show_detail_show = large_show_blank;
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
