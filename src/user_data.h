#pragma once
#define USER_INFO_PERSIST_KEY 10
#define USER_SHOWS_PERSIST_KEY 20
#define USER_CHANNELS_PERSIST_KEY 1000
#define AMOUNT_OF_SHOWS_AVAILABLE 24
#define AMOUNT_OF_CHANNELS_AVAILABLE 130

typedef struct Channel {
    char id[1][6];
    char name[1][10];
    uint8_t number;
} Channel;

typedef struct Show {
    bool is_new;
    char name[1][32];
    Channel channel;
    bool exists;
} Show;

typedef struct LargeShow {
    Show base_show;
    time_t start;
    time_t end;
} LargeShow;

Show user_data_get_show_from_index(int index);
void user_data_set_show_for_index(Show show, int index);
uint8_t amount_of_shows_that_exist(Show shows[AMOUNT_OF_SHOWS_AVAILABLE]);
Channel user_data_get_channel_for_index(int index);
uint8_t user_data_get_amount_of_channels();
void user_data_set_channel_for_index(Channel channel, int index, bool first_load);
void user_data_load();
void user_data_save();
