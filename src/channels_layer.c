#include <pebble.h>
#include "channels_layer.h"
#include "user_data.h"
#include "data_framework.h"
/*
Window *channels_main_window;
MenuLayer *channels_menu_layer;
UserShows shows;

uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
	return 1;
}

uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
	switch (section_index) {
		case 0:{
			int amount = 0;
			for(int i = 0; i < AMOUNT_OF_channels_AVAILABLE; i++){
				if(shows.exists[i]){
					amount++;
				}
			}
			if(amount != 6){
				amount++;
			}
			return amount;
		}
		default:
			return 0;
	}
}

int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
	return MENU_CELL_BASIC_HEADER_HEIGHT;
}

void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
	switch (section_index) {
		case 0:
			menu_cell_basic_header_draw(ctx, cell_layer, "Current Shows");
			break;
	}
}

void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
	if(shows.exists[cell_index->row]){
		menu_cell_basic_draw(ctx, cell_layer, shows.current[cell_index->row].name[0], shows.current[cell_index->row].channel.name[0], NULL);
	}
	else{
		menu_cell_basic_draw(ctx, cell_layer, "Add new show", "Click to find one", NULL);
	}
}

void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {

}

void channels_layer_main_window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);

	channels_menu_layer = menu_layer_create(bounds);
	menu_layer_set_callbacks(channels_menu_layer, NULL, (MenuLayerCallbacks){
		.get_num_sections = menu_get_num_sections_callback,
		.get_num_rows = menu_get_num_rows_callback,
		.get_header_height = menu_get_header_height_callback,
		.draw_header = menu_draw_header_callback,
		.draw_row = menu_draw_row_callback,
		.select_click = menu_select_callback,
	});

	menu_layer_set_click_config_onto_window(channels_menu_layer, window);
	menu_layer_set_highlight_colors(channels_menu_layer, GColorRed, GColorWhite);

	layer_add_child(window_layer, menu_layer_get_layer(channels_menu_layer));

	shows = user_data_get_user_shows();
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
	window_stack_push(channels_main_window, true);
}

void channels_layer_deinit() {
	window_destroy(channels_main_window);
}

Window* channels_layer_get_window(){
	return channels_main_window;
}
*/
