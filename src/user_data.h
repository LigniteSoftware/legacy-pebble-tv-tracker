#pragma once
#define USER_INFO_PERSIST_KEY 10
#define USER_SHOWS_PERSIST_KEY 20
#define USER_CHANNELS_PERSIST_KEY 1000
#define AMOUNT_OF_SHOWS_AVAILABLE 25
#define AMOUNT_OF_CHANNELS_AVAILABLE 130

//Edwin, why use struct?
//Because I prefer it, okay? They're nice and clean.
//Okay.

typedef struct Channel {
    char id[1][6];
    char name[1][10];
    uint8_t number;
} Channel;

typedef struct Channels {
    Channel current[AMOUNT_OF_CHANNELS_AVAILABLE];
    uint8_t exists;
} Channels;

typedef struct Show {
    bool is_new;
    char name[1][32];
    Channel channel;
} Show;

typedef struct Shows {
    Show current[AMOUNT_OF_SHOWS_AVAILABLE];
    bool exists[AMOUNT_OF_SHOWS_AVAILABLE];
} Shows;

typedef struct LargeShow {
    Show base_show;
    time_t start;
    time_t end;
} LargeShow;

Shows user_data_get_shows();
void user_data_set_shows(Shows shows);
uint8_t amount_of_shows_that_exist(Shows shows);
Channels user_data_get_channels();
void user_data_set_channels(Channels channels);
void user_data_load();
void user_data_save();
