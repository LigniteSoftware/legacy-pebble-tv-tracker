#include <pebble.h>
#include <lignite.h>
#include "main_window.h"

int main() {
	data_framework_init();
	int resultLoad = data_framework_load_settings();
	APP_LOG(APP_LOG_LEVEL_INFO, "%d bytes loaded from settings.", resultLoad);
    main_window_init();
    window_stack_push(main_window_get_window(), true);
    app_event_loop();
    main_window_destroy();
    int resultSave = data_framework_save_settings();
    APP_LOG(APP_LOG_LEVEL_INFO, "%d bytes saved to settings", resultSave);
    return 1337;
}
