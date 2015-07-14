#pragma once
#define USER_INFO_PERSIST_KEY 10

typedef struct Channel {
    char id[1][6];
    char name[1][10];
} Channel;

typedef struct Show {
    char name[1][16];
    bool only_new;
    Channel channel;
} Show;

typedef struct UserInfo {
    bool loggedIn;
    char username[1][20];
    char accessToken[1][20];
} UserInfo;

typedef struct UserShows {
    Show current[6];
} UserShows;

bool user_data_is_logged_in();
UserInfo user_data_get_user_info();
UserShows user_data_get_user_shows();
void user_data_load();
void user_data_save();
