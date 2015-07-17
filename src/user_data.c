#include <pebble.h>
#include <data_framework.h>
#include "user_data.h"

UserInfo current_info;
UserShows current_shows;
LoginHandler current_login_handler;

bool user_data_is_logged_in(){
	return current_info.loggedIn;
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

void user_data_register_login_handler(LoginHandler handler){
	current_login_handler = handler;
}

void user_data_update_info(UserInfo new_info){
	current_info = new_info;
	current_login_handler(current_info.loggedIn);
}

void user_data_load(){
	int info_result = persist_read_data(USER_INFO_PERSIST_KEY, &current_info, sizeof(current_info));
	int show_result = persist_read_data(USER_SHOWS_PERSIST_KEY, &current_shows, sizeof(current_shows));
	APP_LOG(APP_LOG_LEVEL_INFO, "Read %d bytes from info, and %d from shows.", info_result, show_result);
}

void user_data_save(){
	int info_result = persist_write_data(USER_INFO_PERSIST_KEY, &current_info, sizeof(current_info));
	int show_result = persist_write_data(USER_SHOWS_PERSIST_KEY, &current_shows, sizeof(current_shows));
	APP_LOG(APP_LOG_LEVEL_INFO, "Wrote %d bytes to info, and %d to shows.", info_result, show_result);
}
