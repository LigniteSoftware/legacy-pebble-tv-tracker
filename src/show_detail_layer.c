#include <pebble.h>
#include "show_detail_layer.h"
#include "data_framework.h"
#include "other.h"
#include "notification.h"
#include "user_data.h"
#include "main_window.h"
#include "shows_layer.h"
#include "animations.h"

static const LargeShow large_show_blank;

int show_detail_index = 0, show_detail_index_max = 0, show_detail_index_min = 0;
int index_set = 0;
bool is_in_action_mode = false, was_subscribing = false;
Window *show_detail_window;
LargeShow show_detail_show;
Layer *show_detail_graphics_layer, *show_detail_options_layer;
GBitmap *hamburger_icon, *checkmark_icon, *up_icon, *down_icon, *tv_icon, *start_icon, *end_icon, *success_background, *failed_background;
ActionBarLayer *show_detail_action_bar;
TextLayer *show_name_layer, *show_channel_layer, *show_start_layer, *show_end_layer, *show_new_layer;
BitmapLayer *tv_icon_layer, *end_icon_layer, *start_icon_layer;
NextShowCallback next_show_callback;
Notification *status_success_notification, *status_failed_notification;

void show_detail_register_next_show_callback(NextShowCallback callback){
	next_show_callback = callback;
}

bool show_detail_is_already_subscribed(){
	return (show_detail_show.start == 0);
}

void show_detail_set_large_show(LargeShow show){
	APP_LOG(APP_LOG_LEVEL_INFO, "Got show with name: %s", show.base_show.name[0]);
	show_detail_show = show;
}

void show_detail_set_show(Show show){
	memcpy(&show_detail_show.base_show, &show, sizeof(show));
}

void show_detail_set_min_max(int min, int max){
	show_detail_index_max = max;
	show_detail_index_min = min;
}

void show_detail_set_index(int index){
	index_set = index;
}

void show_detail_update(ScrollDirection direction){
	struct tm *start_t = localtime(&show_detail_show.start);
	static char start_buffer[] = "Hello start how are you";
	strftime(start_buffer, sizeof(start_buffer), "%D %H:%M", start_t);

	if(direction != ScrollDirectionNone){
		AppDataPoint *start_layer_update = malloc(sizeof(AppDataPoint));
		start_layer_update->layer = show_start_layer;
		start_layer_update->is_name = false;
		strcpy(start_layer_update->new_text[0], start_buffer);
		ask_for_scroll(start_layer_update, direction);
	}
	else{
		text_layer_set_text(show_start_layer, start_buffer);
	}

	struct tm *end_t = localtime(&show_detail_show.end);
	static char end_buffer[] = "Hello start how are you";
	strftime(end_buffer, sizeof(end_buffer), "%D %H:%M", end_t);

	if(direction != ScrollDirectionNone){
		AppDataPoint *end_layer_update = malloc(sizeof(AppDataPoint));
		end_layer_update->layer = show_end_layer;
		end_layer_update->is_name = false;
		strcpy(end_layer_update->new_text[0], end_buffer);
		ask_for_scroll(end_layer_update, direction);
	}
	else{
		text_layer_set_text(show_end_layer, end_buffer);
	}

	text_layer_set_font(show_name_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	layer_set_frame(text_layer_get_layer(show_name_layer), GRect(10, 26, 144-ACTION_BAR_WIDTH-20, 54));

	if(direction != ScrollDirectionNone){
		AppDataPoint *name_layer_update = malloc(sizeof(AppDataPoint));
		name_layer_update->layer = show_name_layer;
		name_layer_update->is_name = true;
		strcpy(name_layer_update->new_text[0], show_detail_show.base_show.name[0]);
		ask_for_scroll(name_layer_update, direction);
	}
	else{
		text_layer_set_text(show_name_layer, show_detail_show.base_show.name[0]);
	}

	text_layer_set_text(show_channel_layer, show_detail_show.base_show.channel.name[0]);

	if(direction != ScrollDirectionNone){
		AppDataPoint *new_layer_update = malloc(sizeof(AppDataPoint));
		new_layer_update->layer = show_new_layer;
		new_layer_update->is_name = false;
		strcpy(new_layer_update->new_text[0], show_detail_show.base_show.is_new ? "New" : "Rerun");
		ask_for_scroll(new_layer_update, direction);
	}
	else{
		text_layer_set_text(show_new_layer, show_detail_show.base_show.is_new ? "New" : "Rerun");
		int length = strlen(show_detail_show.base_show.name[0]);
		if(length < 12){
			layer_set_frame(text_layer_get_layer(show_name_layer), GRect(10, 36, 144-ACTION_BAR_WIDTH-20, 34));
			if(length < 10){
				text_layer_set_font(show_name_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
			}
		}
	}

	if(show_detail_is_already_subscribed()){
		text_layer_set_text(show_start_layer, "Subscribed");
		text_layer_set_text(show_end_layer, "recently");
		text_layer_set_text(show_new_layer, "All Shows");
		action_bar_layer_set_icon_animated(show_detail_action_bar, BUTTON_ID_DOWN, NULL, true);
		action_bar_layer_set_icon_animated(show_detail_action_bar, BUTTON_ID_UP, NULL, true);
	}
}

void show_detail_next_show(ClickRecognizerRef referee, void *ctx){
	if(show_detail_is_already_subscribed()){
		return;
	}
	show_detail_index++;
	if(show_detail_index > show_detail_index_max-1){
		show_detail_index = 0;
	}
	memcpy(&show_detail_show, next_show_callback(show_detail_index), sizeof(LargeShow));
	show_detail_update(ScrollDirectionUp);
}

void show_detail_previous_show(ClickRecognizerRef referee, void *ctx){
	if(show_detail_is_already_subscribed()){
		return;
	}
	show_detail_index--;
	if(show_detail_index < 0){
		show_detail_index = show_detail_index_max;
	}
	memcpy(&show_detail_show, next_show_callback(show_detail_index), sizeof(LargeShow));
	show_detail_update(ScrollDirectionDown);
}

void subscribe_to_show(char show[1][32], char channel[1][10]){
	if(shows_layer_is_full()){
		ActionStatus status;
		strcpy(status.error[0], "No more slots");
		status.success = false;
		show_detail_status_callback(status);
		return;
	}
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);

	if (iter == NULL) {
		return;
	}

	dict_write_uint16(iter, APP_KEY_MESSAGE_TYPE, APP_KEY_SHOW_SUBSCRIBE);
	dict_write_cstring(iter, APP_KEY_SHOW_NAME, show[0]);
	dict_write_cstring(iter, APP_KEY_CHANNEL_NAME, channel[0]);

	app_message_outbox_send();

	was_subscribing = true;
}

void unsubscribe_from_show(char show[1][32]){
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);

	if (iter == NULL) {
		return;
	}

	dict_write_uint16(iter, APP_KEY_MESSAGE_TYPE, APP_KEY_SHOW_UNSUBSCRIBE);
	dict_write_cstring(iter, APP_KEY_SHOW_NAME, show[0]);
	dict_write_end(iter);

	app_message_outbox_send();

	was_subscribing = false;
}

void show_detail_sub_action(ClickRecognizerRef referee, void *ctx){
	if(!is_in_action_mode){
		animate_layer(show_detail_options_layer, &GRect(144, 0, 144, 168), &GRect(25, 0, 144, 168), 300, 10);
	}
	else{
		if(show_detail_is_already_subscribed()){
			unsubscribe_from_show(show_detail_show.base_show.name);
		}
		else{
			subscribe_to_show(show_detail_show.base_show.name, show_detail_show.base_show.channel.name);
		}
		return;
	}
	is_in_action_mode = true;
	action_bar_layer_set_icon_animated(show_detail_action_bar, BUTTON_ID_SELECT, !is_in_action_mode ? hamburger_icon : checkmark_icon, true);
}

void back_button_override(ClickRecognizerRef eree, void *ctx){
	if(is_in_action_mode){
		animate_layer(show_detail_options_layer, &GRect(25, 0, 144, 168), &GRect(144, 0, 144, 168), 300, 10);
		is_in_action_mode = false;
	}
	else{
		window_stack_pop(true);
	}
	action_bar_layer_set_icon_animated(show_detail_action_bar, BUTTON_ID_SELECT, !is_in_action_mode ? hamburger_icon : checkmark_icon, true);
}

void show_detail_click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_DOWN, show_detail_next_show);
	window_single_click_subscribe(BUTTON_ID_SELECT, show_detail_sub_action);
	window_single_click_subscribe(BUTTON_ID_UP, show_detail_previous_show);
	window_single_click_subscribe(BUTTON_ID_BACK, back_button_override);
}

void show_detail_graphics_proc(Layer *layer, GContext *ctx){
	graphics_context_set_fill_color(ctx, GColorRed);
	graphics_fill_rect(ctx, GRect(0, 0, 144, 30), 0, GCornerNone);
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_rect(ctx, GRect(8, 80, 98, 2), 0, GCornerNone);
	graphics_fill_rect(ctx, GRect(8, 124, 98, 2), 0, GCornerNone);
}

void show_detail_options_proc(Layer *layer, GContext *ctx){
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_rect(ctx, GRect(0, 0, 144, 168), 0, GCornerNone);
	graphics_context_set_stroke_color(ctx, GColorWhite);
	graphics_draw_text(ctx, show_detail_is_already_subscribed() ? "Unsubscribe" : shows_layer_is_full() ? "6/6 Full" : "Subscribe", fonts_get_system_font(FONT_KEY_GOTHIC_24), GRect(8, 64, 144, 168), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
}

void send_callback(){
	if(was_subscribing){
		APP_LOG(APP_LOG_LEVEL_INFO, "Adding show.");
		Show newShow;
		memcpy(&newShow, &show_detail_show.base_show, sizeof(newShow));
		shows_layer_add_show(newShow);
	}
	else{
		APP_LOG(APP_LOG_LEVEL_INFO, "Was unsubbing");
		shows_layer_delete(index_set);
	}
	window_stack_pop_all(true);
	window_stack_push(shows_layer_get_window(), true);
}

void show_detail_status_callback(ActionStatus status){
	APP_LOG(APP_LOG_LEVEL_INFO, "Got status! %s with error: %s", status.success ? "Good to go" : "Failed", status.error[0]);
	if(status.success){
		vibes_double_pulse();
		AppTimer *calltimer = app_timer_register(100, send_callback, NULL);
	}
	else{
		static char failed_buffer[180];
		snprintf(failed_buffer, sizeof(failed_buffer), "Action failed with error: %s. Sorry.\n(This will auto-dismiss)", status.error[0]);
		text_layer_set_text(status_failed_notification->content, failed_buffer);
		notification_push(status_failed_notification, 10000);
		vibes_long_pulse();
	}
}

void show_detail_window_load(Window *window){
	Layer *window_layer = window_get_root_layer(window);

	up_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_UP_ICON);
	down_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DOWN_ICON);
	hamburger_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HAMBURGER_ICON);
	checkmark_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CHECKMARK_ICON);
	tv_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TV_ICON);
	start_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_START_ICON);
	end_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_END_ICON);
	success_background = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SUCCESS_BACKGROUND);
	failed_background = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FAILED_BACKGROUND);

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

	show_detail_graphics_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(show_detail_graphics_layer, show_detail_graphics_proc);
	layer_add_child(window_layer, show_detail_graphics_layer);

	tv_icon_layer = bitmap_layer_create(GRect(4, 2, 24, 24));
	bitmap_layer_set_bitmap(tv_icon_layer, tv_icon);
	layer_add_child(window_layer, bitmap_layer_get_layer(tv_icon_layer));

	show_channel_layer = text_layer_create(GRect(14, 4, 144-ACTION_BAR_WIDTH-14, 168));
	text_layer_set_text_alignment(show_channel_layer, GTextAlignmentCenter);
	text_layer_set_font(show_channel_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text_color(show_channel_layer, GColorWhite);
	text_layer_set_background_color(show_channel_layer, GColorClear);
	layer_add_child(window_layer, text_layer_get_layer(show_channel_layer));

	show_start_layer = text_layer_create(GRect(10, 84, 144-ACTION_BAR_WIDTH-10, 16));
	text_layer_set_text_alignment(show_start_layer, GTextAlignmentCenter);
	text_layer_set_font(show_start_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
	text_layer_set_background_color(show_start_layer, GColorClear);
	layer_add_child(window_layer, text_layer_get_layer(show_start_layer));

	show_end_layer = text_layer_create(GRect(10, 104, 144-ACTION_BAR_WIDTH-10, 16));
	text_layer_set_text_alignment(show_end_layer, GTextAlignmentCenter);
	text_layer_set_font(show_end_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
	text_layer_set_background_color(show_end_layer, GColorClear);
	layer_add_child(window_layer, text_layer_get_layer(show_end_layer));

	show_new_layer = text_layer_create(GRect(0, 126, 144-ACTION_BAR_WIDTH, 26));
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
	action_bar_layer_set_icon_animated(show_detail_action_bar, BUTTON_ID_SELECT, hamburger_icon, true);
	action_bar_layer_set_icon_animated(show_detail_action_bar, BUTTON_ID_DOWN, down_icon, true);

	status_success_notification = notification_create(window, "Success", "Action completed successfully, just thought I'd let you know.\n(This will auto-dismiss)", success_background);
	status_failed_notification = notification_create(window, "Failed", "Okay", failed_background);

	data_framework_status_service_subscribe(show_detail_status_callback);

	show_detail_update(ScrollDirectionNone);
}

void show_detail_window_unload(Window *window){
	action_bar_layer_destroy(show_detail_action_bar);

	text_layer_destroy(show_name_layer);
	text_layer_destroy(show_channel_layer);
	text_layer_destroy(show_new_layer);
	text_layer_destroy(show_start_layer);
	text_layer_destroy(show_end_layer);

	bitmap_layer_destroy(tv_icon_layer);
	bitmap_layer_destroy(start_icon_layer);
	bitmap_layer_destroy(end_icon_layer);

	layer_destroy(show_detail_options_layer);
	layer_destroy(show_detail_graphics_layer);

	gbitmap_destroy(hamburger_icon);
	gbitmap_destroy(checkmark_icon);
	gbitmap_destroy(up_icon);
	gbitmap_destroy(down_icon);
	gbitmap_destroy(tv_icon);
	gbitmap_destroy(start_icon);
	gbitmap_destroy(end_icon);
	gbitmap_destroy(success_background);
	gbitmap_destroy(failed_background);

	notification_destroy(status_success_notification);
	notification_destroy(status_failed_notification);

	is_in_action_mode = false;
	was_subscribing = false;

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
