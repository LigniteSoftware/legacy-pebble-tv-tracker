#include <pebble.h>
#include <data_framework.h>
#include "user_data.h"

UserInfo current_info;
UserShows current_shows;

bool user_data_is_logged_in(){
	return true;
}

UserInfo user_data_get_user_info(){
	return current_info;
}

UserShows user_data_get_user_shows(){
	UserShows fake;
	for(int i = 0; i < AMOUNT_OF_SHOWS_AVAILABLE; i++){
		fake.exists[i] = false;
	}
	strcpy(fake.current[0].name[0], "Chuck");
	strcpy(fake.current[0].channel.name[0], "NBC");
	fake.exists[0] = true;
	return fake;
}

void user_data_load(){

}

void user_data_save(){

}
