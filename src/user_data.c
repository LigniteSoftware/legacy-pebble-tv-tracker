#include <pebble.h>
#include <data_framework.h>
#include "user_data.h"

#define DEBUG

Shows current_shows;
Channels current_channels;

Shows user_data_get_shows(){
	return current_shows;
}

void user_data_set_shows(Shows shows){
	current_shows = shows;
}

uint8_t amount_of_shows_that_exist(Shows shows){
	int i = 0;
	while(shows.exists[i]){
		i++;
	}
	return i;
}

Channels user_data_get_channels(){
	return current_channels;
}

void user_data_set_channels(Channels channels){
	current_channels = channels;
}

void user_data_load(){
	for(int i = 0; i < amount_of_shows_that_exist(current_shows); i++){
		int show_result = persist_read_data(USER_SHOWS_PERSIST_KEY+i, &current_shows.current[i], sizeof(Show));
		#ifdef DEBUG
			APP_LOG(APP_LOG_LEVEL_INFO, "Read %d bytes from show %d.", show_result, i);
		#endif
	}
	for(int i = 0; i < AMOUNT_OF_CHANNELS_AVAILABLE; i++){
		int channel_result = persist_read_data(USER_CHANNELS_PERSIST_KEY+i, &current_channels.current[i], sizeof(Channel));
		#ifdef DEBUG
			APP_LOG(APP_LOG_LEVEL_INFO, "Read %d bytes from channel %d", channel_result, i);
		#endif
	}
}

void user_data_save(){
	for(int i = 0; i < amount_of_shows_that_exist(current_shows); i++){
		int show_result = persist_write_data(USER_SHOWS_PERSIST_KEY+i, &current_shows.current[i], sizeof(Show));
		#ifdef DEBUG
			APP_LOG(APP_LOG_LEVEL_INFO, "Wrote %d bytes to show %d.", show_result, i);
		#endif
	}
	for(int i = 0; i < AMOUNT_OF_CHANNELS_AVAILABLE; i++){
		int channel_result = persist_write_data(USER_CHANNELS_PERSIST_KEY+i, &current_channels.current[i], sizeof(Channel));
		#ifdef DEBUG
			APP_LOG(APP_LOG_LEVEL_INFO, "Wrote %d bytes to channel %d", channel_result, i);
		#endif
	}
}
