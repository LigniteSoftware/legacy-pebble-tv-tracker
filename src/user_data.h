#pragma once
#define USER_INFO_PERSIST_KEY 10
#define USER_SHOWS_PERSIST_KEY 20
#define AMOUNT_OF_SHOWS_AVAILABLE 6

typedef struct Channel {
    char id[1][6];
    char name[1][10];
    uint8_t number;
} Channel;

typedef struct Show {
    bool is_new;
    char name[1][16];
    Channel channel;
} Show;

typedef struct LargeShow {
    Show base_show;
    time_t start;
    time_t end;
} LargeShow;

typedef struct UserInfo {
    bool loggedIn;
    char username[1][16];
    char accessToken[1][10];
} UserInfo;

typedef struct UserShows {
    Show current[6];
    bool exists[6];
} UserShows;

typedef void (*LoginHandler)(bool logged_in);

bool user_data_is_logged_in();
UserInfo user_data_get_user_info();
UserShows user_data_get_user_shows();
void user_data_register_login_handler(LoginHandler handler);
void user_data_update_info(UserInfo new_info);
void user_data_load();
void user_data_save();
