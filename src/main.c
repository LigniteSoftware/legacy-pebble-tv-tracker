#include <pebble.h>
#include <data_framework.h>
#include "main_window.h"
#include "user_data.h"
#include "channels_layer.h"
#include "new_show_layer.h"

int main() {
	data_framework_init();
	user_data_load();

	channels_layer_init();
	new_shows_layer_init();

    main_window_init();
    window_stack_push(main_window_get_window(), true);

    app_event_loop();

	user_data_save();
    main_window_destroy();

    return 1337;
}
