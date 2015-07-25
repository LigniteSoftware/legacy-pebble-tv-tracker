#include <pebble.h>
#include <data_framework.h>
#include "user_data.h"

//#define DEBUG
//#define DEBUG_SMALL

Show current_shows[AMOUNT_OF_SHOWS_AVAILABLE];
Channel current_channels[AMOUNT_OF_CHANNELS_AVAILABLE];

Show user_data_get_show_from_index(int index){
	return current_shows[index];
}

void user_data_set_show_for_index(Show show, int index){
	current_shows[index] = show;
	int show_result = persist_write_data(USER_SHOWS_PERSIST_KEY+index, &current_shows[index], sizeof(Show));
	#ifdef DEBUG_SMALL
		APP_LOG(APP_LOG_LEVEL_INFO, "(Specific) Wrote %d bytes to show %d.", show_result, index);
	#endif
}

uint8_t amount_of_shows_that_exist(Show shows[AMOUNT_OF_SHOWS_AVAILABLE]){
	int i = 0;
	while(shows[i].exists){
		i++;
	}
	return i;
}

Channel user_data_get_channel_for_index(int index){
	return current_channels[index];
}

void user_data_set_channel_for_index(Channel channel, int index, bool first_load){
	current_channels[index] = channel;
	if(first_load){
		int channel_result = persist_write_data(USER_CHANNELS_PERSIST_KEY+index, &current_channels[index], sizeof(Channel));
		#ifdef DEBUG_SMALL
			APP_LOG(APP_LOG_LEVEL_INFO, "(Specific) Wrote %d bytes to channel %d", channel_result, index);
		#endif
	}
}

uint8_t user_data_get_amount_of_channels(){
	uint8_t i = 0;
	while(current_channels[i].number != 0){
		i++;
	}
	return i;
}

void user_data_load(){
	for(int i = 0; i < AMOUNT_OF_SHOWS_AVAILABLE; i++){
		int show_result = persist_read_data(USER_SHOWS_PERSIST_KEY+i, &current_shows[i], sizeof(Show));
		#ifdef DEBUG
			APP_LOG(APP_LOG_LEVEL_INFO, "Read %d bytes from show %d.", show_result, i);
		#endif
	}
	for(int i = 0; i < AMOUNT_OF_CHANNELS_AVAILABLE; i++){
		int channel_result = persist_read_data(USER_CHANNELS_PERSIST_KEY+i, &current_channels[i], sizeof(Channel));
		#ifdef DEBUG
			APP_LOG(APP_LOG_LEVEL_INFO, "Read %d bytes from channel %d", channel_result, i);
		#endif
	}
}

void user_data_save(){
	return;
	for(int i = 0; i < amount_of_shows_that_exist(current_shows); i++){
		int show_result = persist_write_data(USER_SHOWS_PERSIST_KEY+i, &current_shows[i], sizeof(Show));
		#ifdef DEBUG
			APP_LOG(APP_LOG_LEVEL_INFO, "Wrote %d bytes to show %d.", show_result, i);
		#endif
	}
	for(int i = 0; i < AMOUNT_OF_CHANNELS_AVAILABLE; i++){
		int channel_result = persist_write_data(USER_CHANNELS_PERSIST_KEY+i, &current_channels[i], sizeof(Channel));
		#ifdef DEBUG
			APP_LOG(APP_LOG_LEVEL_INFO, "Wrote %d bytes to channel %d", channel_result, i);
		#endif
	}
}
