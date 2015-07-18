#include <pebble.h>
#include "show_detail_layer.h"
#include "data_framework.h"

static const LargeShow large_show_blank;

int show_detail_index = 0;

Window *show_detail_window;
LargeShow show_detail_show;
Layer *show_detail_graphics_layer, *show_detail_options_layer;
GBitmap *subscribe_icon, *unsubscribe_icon, *up_icon, *down_icon, *tv_icon, *start_icon, *end_icon;
ActionBarLayer *show_detail_action_bar;
TextLayer *show_name_layer, *show_channel_layer, *show_start_layer, *show_end_layer, *show_new_layer;
BitmapLayer *tv_icon_layer, *end_icon_layer, *start_icon_layer;
NextShowCallback next_show_callback;

void show_detail_register_next_show_callback(NextShowCallback callback){
	next_show_callback = callback;
}

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

void show_detail_update(){
	struct tm *start_t = localtime(&show_detail_show.start);
	static char start_buffer[] = "Hello start how are you";
	strftime(start_buffer, sizeof(start_buffer), "%D %H:%M", start_t);
	text_layer_set_text(show_start_layer, start_buffer);

	struct tm *end_t = localtime(&show_detail_show.end);
	static char end_buffer[] = "Hello start how are you";
	strftime(end_buffer, sizeof(end_buffer), "%D %H:%M", end_t);
	text_layer_set_text(show_end_layer, end_buffer);

	text_layer_set_font(show_name_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	layer_set_frame(text_layer_get_layer(show_name_layer), GRect(10, 26, 144-ACTION_BAR_WIDTH-20, 54));

	int length = strlen(show_detail_show.base_show.name[0]);
	if(length < 12){
		layer_set_frame(text_layer_get_layer(show_name_layer), GRect(10, 36, 144-ACTION_BAR_WIDTH-20, 34));
		if(length < 10){
			text_layer_set_font(show_name_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
		}
	}

	text_layer_set_text(show_name_layer, show_detail_show.base_show.name[0]);
	text_layer_set_text(show_channel_layer, show_detail_show.base_show.channel.name[0]);
	text_layer_set_text(show_new_layer, show_detail_show.base_show.is_new ? "New" : "Rerun");

	action_bar_layer_set_icon_animated(show_detail_action_bar, BUTTON_ID_SELECT, show_detail_is_already_subscribed() ? unsubscribe_icon : subscribe_icon, true);
}

void show_detail_next_show(ClickRecognizerRef referee, void *ctx){
	show_detail_index++;
	memcpy(&show_detail_show, next_show_callback(show_detail_index), sizeof(LargeShow));
	show_detail_update();
}

void show_detail_previous_show(ClickRecognizerRef referee, void *ctx){
	show_detail_index--;
	memcpy(&show_detail_show, next_show_callback(show_detail_index), sizeof(LargeShow));
	show_detail_update();
}

void show_detail_sub_action(ClickRecognizerRef referee, void *ctx){

}

void show_detail_click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_DOWN, show_detail_next_show);
	window_single_click_subscribe(BUTTON_ID_SELECT, show_detail_sub_action);
	window_single_click_subscribe(BUTTON_ID_UP, show_detail_previous_show);
}

void show_detail_graphics_proc(Layer *layer, GContext *ctx){
	graphics_context_set_fill_color(ctx, GColorRed);
	graphics_fill_rect(ctx, GRect(0, 0, 144, 30), 0, GCornerNone);
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_rect(ctx, GRect(8, 80, 98, 2), 0, GCornerNone);
	graphics_fill_rect(ctx, GRect(8, 124, 98, 2), 0, GCornerNone);
}

void show_detail_window_load(Window *window){
	Layer *window_layer = window_get_root_layer(window);

	show_detail_graphics_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(show_detail_graphics_layer, show_detail_graphics_proc);
	layer_add_child(window_layer, show_detail_graphics_layer);

	up_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_UP_ICON);
	down_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DOWN_ICON);
	subscribe_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SUB_ICON);
	unsubscribe_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_UNSUB_ICON);
	tv_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TV_ICON);
	start_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_START_ICON);
	end_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_END_ICON);

	tv_icon_layer = bitmap_layer_create(GRect(4, 2, 24, 24));
	bitmap_layer_set_bitmap(tv_icon_layer, tv_icon);
	layer_add_child(window_layer, bitmap_layer_get_layer(tv_icon_layer));

	start_icon_layer = bitmap_layer_create(GRect(6, 86, 14, 14));
	bitmap_layer_set_bitmap(start_icon_layer, start_icon);
	layer_add_child(window_layer, bitmap_layer_get_layer(start_icon_layer));

	end_icon_layer = bitmap_layer_create(GRect(6, 106, 14, 14));
	bitmap_layer_set_bitmap(end_icon_layer, end_icon);
	layer_add_child(window_layer, bitmap_layer_get_layer(end_icon_layer));

	show_name_layer = text_layer_create(GRect(10, 26, 144-ACTION_BAR_WIDTH-20, 54));
	text_layer_set_text_alignment(show_name_layer, GTextAlignmentCenter);
	text_layer_set_font(show_name_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_background_color(show_name_layer, GColorClear);
	layer_add_child(window_layer, text_layer_get_layer(show_name_layer));

	show_channel_layer = text_layer_create(GRect(14, 4, 144-ACTION_BAR_WIDTH-14, 168));
	text_layer_set_text_alignment(show_channel_layer, GTextAlignmentCenter);
	text_layer_set_font(show_channel_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text_color(show_channel_layer, GColorWhite);
	text_layer_set_background_color(show_channel_layer, GColorClear);
	layer_add_child(window_layer, text_layer_get_layer(show_channel_layer));

	show_start_layer = text_layer_create(GRect(10, 84, 144-ACTION_BAR_WIDTH-10, 168));
	text_layer_set_text_alignment(show_start_layer, GTextAlignmentCenter);
	text_layer_set_font(show_start_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
	text_layer_set_background_color(show_start_layer, GColorClear);
	layer_add_child(window_layer, text_layer_get_layer(show_start_layer));

	show_end_layer = text_layer_create(GRect(10, 104, 144-ACTION_BAR_WIDTH-10, 168));
	text_layer_set_text_alignment(show_end_layer, GTextAlignmentCenter);
	text_layer_set_font(show_end_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
	text_layer_set_background_color(show_end_layer, GColorClear);
	layer_add_child(window_layer, text_layer_get_layer(show_end_layer));

	show_new_layer = text_layer_create(GRect(0, 126, 144-ACTION_BAR_WIDTH, 168));
	text_layer_set_text_alignment(show_new_layer, GTextAlignmentCenter);
	text_layer_set_font(show_new_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_background_color(show_new_layer, GColorClear);
	layer_add_child(window_layer, text_layer_get_layer(show_new_layer));

	show_detail_options_layer = layer_create(GRect(144, 0, 144, 168));
	layer_set_update_proc(show_detail_options_layer, show_detail_options_proc);
	layer_add_child(window_layer, show_detail_options_layer);

	show_detail_action_bar = action_bar_layer_create();
	action_bar_layer_add_to_window(show_detail_action_bar, window);
	action_bar_layer_set_click_config_provider(show_detail_action_bar, show_detail_click_config_provider);

	action_bar_layer_set_icon_animated(show_detail_action_bar, BUTTON_ID_UP, up_icon, true);
	action_bar_layer_set_icon_animated(show_detail_action_bar, BUTTON_ID_DOWN, down_icon, true);

	show_detail_update();
}

void show_detail_window_unload(Window *window){
	action_bar_layer_destroy(show_detail_action_bar);

	text_layer_destroy(show_name_layer);
	text_layer_destroy(show_channel_layer);
	text_layer_destroy(show_new_layer);

	bitmap_layer_destroy(tv_icon_layer);
	bitmap_layer_destroy(start_icon_layer);
	bitmap_layer_destroy(end_icon_layer);

	gbitmap_destroy(subscribe_icon);
	gbitmap_destroy(unsubscribe_icon);
	gbitmap_destroy(up_icon);
	gbitmap_destroy(down_icon);
	gbitmap_destroy(tv_icon);
	gbitmap_destroy(start_icon);
	gbitmap_destroy(end_icon);

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
