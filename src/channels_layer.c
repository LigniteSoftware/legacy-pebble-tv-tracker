#include <pebble.h>
#include "channels_layer.h"
#include "user_data.h"
#include "data_framework.h"
#include "new_show_layer.h"

Window *channels_main_window;
MenuLayer *channels_menu_layer;
Layer *loading_layer;
Shows shows;
Channels channels_layer;
int channel_stack_count = 0;
bool channels_data_has_begun = false;
static char number_buffers[AMOUNT_OF_CHANNELS][4];

void channels_layer_add_channel(Channel channel){
	memcpy(&channels_layer.current[channel_stack_count], &channel, sizeof(Channel));
	channels_data_has_begun = true;
	if(channel_stack_count+1 < AMOUNT_OF_CHANNELS){
		channel_stack_count++;
		channels_layer.exists++;
	}
	else{
		APP_LOG(APP_LOG_LEVEL_INFO, "Amount of channels exceeded in stack, sorry");
	}
	layer_mark_dirty(loading_layer);
	menu_layer_reload_data(channels_menu_layer);

	if(channel_stack_count == 117){
		layer_set_hidden(loading_layer, true);
		user_data_set_channels(channels_layer);
		user_data_save();
		//TODO: Animate
	}
}

uint16_t channels_menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
	return 1;
}

uint16_t channels_menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
	switch (section_index) {
		case 0:{
			return channel_stack_count+1;
		}
		default:
			return 0;
	}
}

int16_t channels_menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
	return MENU_CELL_BASIC_HEADER_HEIGHT;
}

void channels_menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
	switch (section_index) {
		case 0:
			menu_cell_basic_header_draw(ctx, cell_layer, "Channels");
			break;
	}
}

void channels_menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
	if(!channels_data_has_begun){
		menu_cell_basic_draw(ctx, cell_layer, "Loading...", NULL, NULL);
		return;
	}
	if(cell_index->row <= (channel_stack_count-1)){
		snprintf(number_buffers[cell_index->row], sizeof(number_buffers[cell_index->row]), "%d", channels_layer.current[cell_index->row].number);
		menu_cell_basic_draw(ctx, cell_layer, channels_layer.current[cell_index->row].name[0], number_buffers[cell_index->row], NULL);
	}
	else{
		menu_cell_basic_draw(ctx, cell_layer, "No more channels.", "Sorry!", NULL);
	}
}

void channels_menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
	new_show_set_channel(channels_layer.current[cell_index->row]);
	window_stack_push(new_shows_layer_get_window(), true);
}

void send_channels_request(){
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);

	if (iter == NULL) {
	  return;
	}

	dict_write_uint16(iter, APP_KEY_MESSAGE_TYPE, APP_KEY_CHANNEL_SEARCH);
	dict_write_end(iter);

	app_message_outbox_send();
}

void loading_proc(Layer *layer, GContext *ctx){
	graphics_context_set_fill_color(ctx, GColorRed);
	graphics_fill_rect(ctx, GRect(0, 0, 144, 168), 0, GCornerNone);
	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_fill_rect(ctx, GRect(10, 114, channel_stack_count, 6), 3, GCornersAll);
	graphics_draw_text(ctx, "Loading channels", fonts_get_system_font(FONT_KEY_GOTHIC_24), GRect(0, 40, 144, 168), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
	static char loading_buffer[] = "200%%";
	snprintf(loading_buffer, sizeof(loading_buffer), "%d of 119", channel_stack_count);
	graphics_draw_text(ctx, loading_buffer, fonts_get_system_font(FONT_KEY_GOTHIC_18), GRect(0, 70, 144, 168), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
	graphics_draw_text(ctx, channels_layer.current[channel_stack_count-1].name[0], fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), GRect(0, 124, 144, 168), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
}

void channels_layer_main_window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);

	channels_layer = user_data_get_channels();

	channels_menu_layer = menu_layer_create(bounds);
	menu_layer_set_callbacks(channels_menu_layer, NULL, (MenuLayerCallbacks){
		.get_num_sections = channels_menu_get_num_sections_callback,
		.get_num_rows = channels_menu_get_num_rows_callback,
		.get_header_height = channels_menu_get_header_height_callback,
		.draw_header = channels_menu_draw_header_callback,
		.draw_row = channels_menu_draw_row_callback,
		.select_click = channels_menu_select_callback,
	});

	menu_layer_set_click_config_onto_window(channels_menu_layer, window);
	menu_layer_set_highlight_colors(channels_menu_layer, GColorRed, GColorWhite);

	layer_add_child(window_layer, menu_layer_get_layer(channels_menu_layer));

	loading_layer = layer_create(GRect(0, 0, 144, 168));
	if(channels_layer.current[10].number == 0){
		send_channels_request();

		layer_set_update_proc(loading_layer, loading_proc);
		layer_add_child(window_layer, loading_layer);
	}
}

void channels_layer_main_window_unload(Window *window) {
	menu_layer_destroy(channels_menu_layer);
	layer_destroy(loading_layer);
}

void channels_layer_init() {
	channels_main_window = window_create();
	window_set_window_handlers(channels_main_window, (WindowHandlers) {
		.load = channels_layer_main_window_load,
		.unload = channels_layer_main_window_unload,
	});
}

void channels_layer_deinit() {
	window_destroy(channels_main_window);
}

Window* channels_layer_get_window(){
	return channels_main_window;
}
