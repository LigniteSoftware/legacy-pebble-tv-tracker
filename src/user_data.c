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
	return current_shows;
}

void user_data_load(){
	
}

void user_data_save(){

}
