#include <pebble.h>
#include "channels_layer.h"
#include "user_data.h"
#include "data_framework.h"
#include "new_show_layer.h"

Window *channels_main_window;
MenuLayer *channels_menu_layer;
UserShows shows;
Channel channels[AMOUNT_OF_CHANNELS];
int channel_stack_count = 0;
bool channels_data_has_begun = false;
static char number_buffers[AMOUNT_OF_CHANNELS][4];

void channels_layer_add_channel(Channel channel){
	channels_data_has_begun = true;
	channels[channel_stack_count] = channel;
	if(channel_stack_count+1 < AMOUNT_OF_CHANNELS){
		channel_stack_count++;
	}
	else{
		APP_LOG(APP_LOG_LEVEL_INFO, "Amount of channels exceeded in stack, sorry");
	}
	layer_mark_dirty(menu_layer_get_layer(channels_menu_layer));
	menu_layer_reload_data(channels_menu_layer);
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
		snprintf(number_buffers[cell_index->row], sizeof(number_buffers[cell_index->row]), "%d", channels[cell_index->row].number);
		menu_cell_basic_draw(ctx, cell_layer, channels[cell_index->row].name[0], number_buffers[cell_index->row], NULL);
	}
	else{
		menu_cell_basic_draw(ctx, cell_layer, "No more channels.", "Sorry!", NULL);
	}
}

void channels_menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
	new_show_set_channel(channels[cell_index->row]);
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

void channels_layer_main_window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);

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

	if(channels[10].number == 0){
		APP_LOG(APP_LOG_LEVEL_INFO, "Requesting channels...");
		send_channels_request();
	}
}

void channels_layer_main_window_unload(Window *window) {
	menu_layer_destroy(channels_menu_layer);
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
