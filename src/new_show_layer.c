#include <pebble.h>
#include "new_show_layer.h"
#include "user_data.h"
#include "data_framework.h"
#include "show_detail_layer.h"

Window *new_shows_main_window;
MenuLayer *new_show_menu_layer;
LargeShow new_shows[MAX_AMOUNT_OF_SHOWS];
int show_stack_count = 0;
bool show_data_has_begun = false;
Channel channel_location;

void new_show_layer_add_show(LargeShow show){
	show_data_has_begun = true;
	new_shows[show_stack_count] = show;
	new_shows[show_stack_count].base_show.channel = channel_location;
	if(show_stack_count+1 < MAX_AMOUNT_OF_SHOWS){
		show_stack_count++;
	}
	else{
		APP_LOG(APP_LOG_LEVEL_INFO, "Amount of new_shows exceeded in stack, sorry");
	}
	layer_mark_dirty(menu_layer_get_layer(new_show_menu_layer));
	menu_layer_reload_data(new_show_menu_layer);
}

uint16_t new_show_menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
	return 1;
}

uint16_t new_show_menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
	switch (section_index) {
		case 0:{
			return show_stack_count+1;
		}
		default:
			return 0;
	}
}

int16_t new_show_menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
	return MENU_CELL_BASIC_HEADER_HEIGHT;
}

void new_show_menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
	switch (section_index) {
		case 0:
			menu_cell_basic_header_draw(ctx, cell_layer, channel_location.name[0]);
			break;
	}
}

void new_show_menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
	if(!show_data_has_begun){
		menu_cell_basic_draw(ctx, cell_layer, "Loading...", NULL, NULL);
		return;
	}
	if(cell_index->row <= (show_stack_count-1)){
		menu_cell_basic_draw(ctx, cell_layer, new_shows[cell_index->row].base_show.name[0], new_shows[cell_index->row].base_show.is_new ? "New" : "Old/Rerun", NULL);
	}
	else{
		menu_cell_basic_draw(ctx, cell_layer, "That's all.", NULL, NULL);
	}
}

LargeShow *new_show_next_show_callback(int index){
	if(index <= show_stack_count){
		return &new_shows[index];
	}
	return NULL;
}

void new_show_menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
	show_detail_set_large_show(new_shows[cell_index->row]);
	show_detail_register_next_show_callback(new_show_next_show_callback);
	show_detail_set_min_max(0, show_stack_count);
	window_stack_push(show_detail_layer_get_window(), true);
}

void new_show_set_channel(Channel channel){
	channel_location = channel;
}

void send_new_shows_request(){
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);

	if (iter == NULL) {
	  return;
	}

	dict_write_uint16(iter, APP_KEY_MESSAGE_TYPE, APP_KEY_SHOW_SEARCH);
	dict_write_cstring(iter, APP_KEY_CHANNEL_ID, channel_location.id[0]);
	dict_write_end(iter);

	app_message_outbox_send();
}

void new_shows_layer_main_window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);

	new_show_menu_layer = menu_layer_create(bounds);
	menu_layer_set_callbacks(new_show_menu_layer, NULL, (MenuLayerCallbacks){
		.get_num_sections = new_show_menu_get_num_sections_callback,
		.get_num_rows = new_show_menu_get_num_rows_callback,
		.get_header_height = new_show_menu_get_header_height_callback,
		.draw_header = new_show_menu_draw_header_callback,
		.draw_row = new_show_menu_draw_row_callback,
		.select_click = new_show_menu_select_callback,
	});

	menu_layer_set_click_config_onto_window(new_show_menu_layer, window);
	menu_layer_set_highlight_colors(new_show_menu_layer, GColorRed, GColorWhite);

	layer_add_child(window_layer, menu_layer_get_layer(new_show_menu_layer));

	send_new_shows_request();
}

void new_shows_layer_main_window_unload(Window *window) {
	menu_layer_destroy(new_show_menu_layer);
	show_stack_count = 0;
	show_data_has_begun = false;
}

void new_shows_layer_init() {
	new_shows_main_window = window_create();
	window_set_window_handlers(new_shows_main_window, (WindowHandlers) {
		.load = new_shows_layer_main_window_load,
		.unload = new_shows_layer_main_window_unload,
	});
}

void new_shows_layer_deinit() {
	window_destroy(new_shows_main_window);
}

Window* new_shows_layer_get_window(){
	return new_shows_main_window;
}
