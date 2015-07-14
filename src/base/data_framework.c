#include <pebble.h>
#include <lignite.h>
#include "data_framework.h"

Settings data_framework_local_settings;
SettingsChangeCallback settings_callbacks[AMOUNT_OF_SETTINGS_CALLBACKS];

void process_tuple(Tuple *t){
    int key = t->key;
    int value = t->value->int32;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Loaded key %d with value %d.", key, value);
    switch (key) {

    }
}

void data_framework_inbox(DictionaryIterator *iter, void *context){
    Tuple *t = dict_read_first(iter);
    if(t){
        process_tuple(t);
    }
    while(t != NULL){
        t = dict_read_next(iter);
        if(t){
            process_tuple(t);
        }
    }
    for(int i = 0; i < AMOUNT_OF_SETTINGS_CALLBACKS; i++){
        settings_callbacks[i](data_framework_local_settings);
    }
}

void data_framework_load_default_settings(){

}
