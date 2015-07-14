#pragma once

#define SETTINGS_KEY 0
#define AMOUNT_OF_SETTINGS_CALLBACKS 1

typedef enum AppKey {
	APP_KEY_TEST = 0
} AppKey;

typedef enum SettingsCallback {
	SETTINGS_CALLBACK_MAIN_WINDOW = 0
} SettingsCallback;

typedef struct Settings {
	bool test;
} Settings;

typedef void (*SettingsChangeCallback)(Settings settings);

void process_tuple(Tuple *t);
void data_framework_inbox(DictionaryIterator *iter, void *context);
void data_framework_load_default_settings();

extern Settings data_framework_local_settings;
extern SettingsChangeCallback settings_callbacks[AMOUNT_OF_SETTINGS_CALLBACKS];
