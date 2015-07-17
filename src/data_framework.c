#include <pebble.h>
#include <ctype.h>
#include "data_framework.h"
#include "user_data.h"
#include "channels_layer.h"
#include "new_show_layer.h"

static const Channel wiped_channel;

UserInfo data_framework_local_info;
Channel data_framework_local_channel;
LargeShow data_framework_local_show;
uint8_t incoming_type = -1;

void process_tuple(Tuple *t){
    int key = t->key;
    int value = t->value->int32;
    switch (key) {
        case APP_KEY_USERNAME:
            incoming_type = 0;
            strcpy(data_framework_local_info.username[0], t->value->cstring);
            break;
        case APP_KEY_ACCESS_TOKEN:
            strcpy(data_framework_local_info.accessToken[0], t->value->cstring);
            break;
        case APP_KEY_CHANNEL_NAME:
            incoming_type = 1;
            strcpy(data_framework_local_channel.name[0], t->value->cstring);
            break;
        case APP_KEY_CHANNEL_ID:
            strcpy(data_framework_local_channel.id[0], t->value->cstring);
            break;
        case APP_KEY_CHANNEL_NUMBER:
            data_framework_local_channel.number = t->value->int32;
            break;
        case APP_KEY_SHOW_NAME:
            incoming_type = 2;
            strncpy(data_framework_local_show.base_show.name[0], t->value->cstring, sizeof(data_framework_local_show.base_show.name[0])-1);
            break;
        case APP_KEY_SHOW_CHANNEL_ID:
            strncpy(data_framework_local_show.base_show.channel.id[0], t->value->cstring, sizeof(data_framework_local_show.base_show.channel.id[0])-1);
            break;
        case APP_KEY_SHOW_NEW:
            data_framework_local_show.base_show.is_new = t->value->int32;
            break;
        case APP_KEY_SHOW_START:
            data_framework_local_show.start = (time_t) t->value->int32;
            break;
        case APP_KEY_SHOW_END:
            data_framework_local_show.end = (time_t) t->value->int32;
            break;
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
    switch(incoming_type){
        //User logged in
        case 0:
            data_framework_local_info.loggedIn = true;
            user_data_update_info(data_framework_local_info);
            vibes_double_pulse();
            break;
        case 1:
            channels_layer_add_channel(data_framework_local_channel);
            data_framework_local_channel = wiped_channel;
            break;
        case 2:
            new_show_layer_add_show(data_framework_local_show);
            break;
    }
}

//Cheers to http://forums.getpebble.com/discussion/comment/132931#Comment_132931
unsigned int data_framework_hex_string_to_uint(char const* hexstring){
    unsigned int result = 0;
    char const *c = hexstring;
    unsigned char thisC;
    while((thisC = *c) != 0){
        thisC = toupper(thisC);
        result <<= 4;
        if(isdigit(thisC)){
            result += thisC - '0';
        }
        else if(isxdigit(thisC)){
            result += thisC - 'A' + 10;
        }
        else{
            APP_LOG(APP_LOG_LEVEL_DEBUG, "ERROR: Unrecognised hex character \"%c\"", thisC);
            return 0;
        }
        ++c;
    }
    return result;
}

void data_framework_init(){
    app_message_register_inbox_received(data_framework_inbox);
    app_message_open(512, 512);
}
