#pragma once

typedef enum AppKey {
    APP_KEY_MESSAGE_TYPE = 1000,
    APP_KEY_CHANNEL_SEARCH = 1001,
    APP_KEY_SHOW_SEARCH = 1002,
    APP_KEY_SHOW_SUBSCRIBE = 1003,
    APP_KEY_SHOW_UNSUBSCRIBE = 1004,
    APP_KEY_ACTION_STATUS = 2000,
    APP_KEY_ACTION_ERROR = 2001,
    APP_KEY_CHANNEL_NAME = 10,
    APP_KEY_CHANNEL_ID = 11,
    APP_KEY_CHANNEL_NUMBER = 12,
    APP_KEY_SHOW_NAME = 20,
    APP_KEY_SHOW_CHANNEL_ID = 21,
    APP_KEY_SHOW_NEW = 22,
    APP_KEY_SHOW_START = 23,
    APP_KEY_SHOW_END = 24
} AppKey;

typedef struct ActionStatus {
    bool success;
    char error[1][80];
} ActionStatus;

typedef void(*ActionStatusCallback)(ActionStatus status);

void data_framework_inbox(DictionaryIterator *iter, void *context);
void data_framework_status_service_subscribe(ActionStatusCallback callback);
unsigned int data_framework_hex_string_to_uint(char const* hexstring);
void data_framework_init();
