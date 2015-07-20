#include <pebble.h>
#include "shows_layer.h"
#include "data_framework.h"
#include "channels_layer.h"
#include "show_detail_layer.h"

Window *shows_main_window;
MenuLayer *shows_menu_layer;
UserShows shows;
LargeShow test_show;

uint16_t shows_menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
	return 1;
}

int shows_get_amount_of_items(){
	int amount = 0;
	for(int i = 0; i < AMOUNT_OF_SHOWS_AVAILABLE; i++){
		if(shows.exists[i]){
			amount++;
		}
	}
	if(amount != 6){
		amount++;
	}
	return amount;
}

uint16_t shows_menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
	switch (section_index) {
		case 0:{
			return shows_get_amount_of_items();
		}
		default:
			return 0;
	}
}

int16_t shows_menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
	return MENU_CELL_BASIC_HEADER_HEIGHT;
}

void shows_menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
	switch (section_index) {
		case 0:
			menu_cell_basic_header_draw(ctx, cell_layer, "Current Shows");
			break;
	}
}

void shows_menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
	APP_LOG(APP_LOG_LEVEL_INFO, "Good good %s, %s", shows.current[cell_index->row].name[0], shows.current[cell_index->row].channel.name[0]);
	if(shows.exists[cell_index->row]){
		menu_cell_basic_draw(ctx, cell_layer, shows.current[cell_index->row].name[0], shows.current[cell_index->row].channel.name[0], NULL);
	}
	else{
		menu_cell_basic_draw(ctx, cell_layer, "Add or browse", NULL, NULL);
	}
}

void shows_layer_delete(int index){
	shows.exists[index] = false;
	user_data_set_user_shows(shows);
	menu_layer_reload_data(shows_menu_layer);
}

LargeShow *get_show(int index){
	test_show.base_show = shows.current[index];
	return &test_show;
}

void shows_menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
	if(cell_index->row == shows_get_amount_of_items()-1){
		window_stack_push(channels_layer_get_window(), true);
	}
	else{
		show_detail_set_show(shows.current[cell_index->row]);
		show_detail_register_next_show_callback(get_show);
		show_detail_set_index(cell_index->row);
		window_stack_push(show_detail_layer_get_window(), true);
	}
}

void shows_layer_add_show(Show show){
	APP_LOG(APP_LOG_LEVEL_INFO, "Adding new show.");
	memcpy(&shows.current[shows_get_amount_of_items()-1], &show, sizeof(Show));
	shows.exists[shows_get_amount_of_items()-1] = true;
	APP_LOG(APP_LOG_LEVEL_INFO, "Got %s, %s", show.name[0], show.channel.name[0]);
	user_data_set_user_shows(shows);
	menu_layer_reload_data(shows_menu_layer);
}

bool shows_layer_is_full(){
	return shows_get_amount_of_items() == AMOUNT_OF_SHOWS_AVAILABLE;
}

void shows_layer_main_window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);

	shows_menu_layer = menu_layer_create(bounds);
	menu_layer_set_callbacks(shows_menu_layer, NULL, (MenuLayerCallbacks){
		.get_num_sections = shows_menu_get_num_sections_callback,
		.get_num_rows = shows_menu_get_num_rows_callback,
		.get_header_height = shows_menu_get_header_height_callback,
		.draw_header = shows_menu_draw_header_callback,
		.draw_row = shows_menu_draw_row_callback,
		.select_click = shows_menu_select_callback,
	});

	menu_layer_set_click_config_onto_window(shows_menu_layer, window);
	menu_layer_set_highlight_colors(shows_menu_layer, GColorRed, GColorWhite);

	layer_add_child(window_layer, menu_layer_get_layer(shows_menu_layer));

	shows = user_data_get_user_shows();
	menu_layer_reload_data(shows_menu_layer);
}

void shows_layer_main_window_unload(Window *window) {
	menu_layer_destroy(shows_menu_layer);
}

void shows_layer_init() {
	shows_main_window = window_create();
	window_set_window_handlers(shows_main_window, (WindowHandlers) {
		.load = shows_layer_main_window_load,
		.unload = shows_layer_main_window_unload,
	});
}

void shows_layer_deinit() {
	window_destroy(shows_main_window);
}

Window* shows_layer_get_window(){
	return shows_main_window;
}
