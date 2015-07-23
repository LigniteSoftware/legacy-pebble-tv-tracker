#include <pebble.h>
#include "other.h"
#include "show_detail_layer.h"

TextLayer *text_layer_init(GRect location, GTextAlignment alignment, GFont *font){
	TextLayer *layer = text_layer_create(location);
	text_layer_set_text_color(layer, GColorWhite);
	text_layer_set_background_color(layer, GColorClear);
	text_layer_set_text_alignment(layer, alignment);
	text_layer_set_font(layer, font);
	return layer;
}

void on_animation_stopped(Animation *anim, bool finished, void *context){
    #ifdef PBL_BW
        property_animation_destroy((PropertyAnimation*) anim);
    #endif
}

void animation_update_handler(struct Animation *animation, const AnimationProgress distance_normalized){
	APP_LOG(APP_LOG_LEVEL_INFO, "again");
	int percent = (distance_normalized/(ANIMATION_NORMALIZED_MAX/100));
	APP_LOG(APP_LOG_LEVEL_INFO, "Percent: %d", percent);
	if(animation_get_context(animation)){
		if(percent > 51){
			TextLayerUpdate *update = animation_get_context(animation);
			text_layer_set_text(update->layer, update->new_text);
		}
	}
}

static const PropertyAnimationImplementation implementation = {
	.base = {
		.update = animation_update_handler
	}
};

void animate_layer(Layer *layer, GRect *start, GRect *finish, int length, int delay, void *data){
    PropertyAnimation *anim = property_animation_create(&implementation, NULL, NULL, NULL);

    animation_set_duration(property_animation_get_animation(anim), length);
    animation_set_delay(property_animation_get_animation(anim), delay);

	AnimationHandlers handlers = {
    	.stopped = (AnimationStoppedHandler) on_animation_stopped
    };
	APP_LOG(APP_LOG_LEVEL_INFO, "schedule");
    animation_schedule(property_animation_get_animation(anim));
}

Animation *animate_layer_return(Layer *layer, GRect *start, GRect *finish, int length, int delay){
    PropertyAnimation *anim = property_animation_create_layer_frame(layer, start, finish);

    animation_set_duration(property_animation_get_animation(anim), length);
    animation_set_delay(property_animation_get_animation(anim), delay);

	AnimationHandlers handlers = {
    	.stopped = (AnimationStoppedHandler) on_animation_stopped
    };
    animation_set_handlers(property_animation_get_animation(anim), handlers, NULL);

    animation_schedule(property_animation_get_animation(anim));
    return (Animation*)anim;
}
