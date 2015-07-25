#include <pebble.h>
#include "other.h"
#include "show_detail_layer.h"
#include "animations.h"

const uint32_t SCROLL_DURATION = 200;
const int16_t SCROLL_DIST_OUT = 10;
const int16_t SCROLL_DIST_IN = 10;

void after_scroll_swap_text(Animation *animation, bool finished, void *context) {
    AppDataPoint *point = context;
    if(point == NULL){
        APP_LOG(APP_LOG_LEVEL_ERROR, "Point is null");
        return;
    }
    text_layer_set_text(point->layer, point->new_text[0]);
    if(point->is_name){
        int length = strlen(point->new_text[0]);
    	if(length < 12){
    		layer_set_frame(text_layer_get_layer(point->layer), GRect(10, 36, 144-ACTION_BAR_WIDTH-20, 34));
    		if(length < 10){
    			text_layer_set_font(point->layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    		}
    	}
    }
}

Animation *create_anim_scroll_out(Layer *layer, uint32_t duration, int16_t dy) {
  GPoint to_origin = GPoint(0, dy);
  Animation *result = (Animation *) property_animation_create_bounds_origin(layer, NULL, &to_origin);
  animation_set_duration(result, duration);
  animation_set_curve(result, AnimationCurveLinear);
  return result;
}

Animation *create_anim_scroll_in(Layer *layer, uint32_t duration, int16_t dy) {
  GPoint from_origin = GPoint(0, dy);
  Animation *result = (Animation *) property_animation_create_bounds_origin(layer, &from_origin, &GPointZero);
  animation_set_duration(result, duration);
  animation_set_curve(result, AnimationCurveEaseOut);
  return result;
}

Animation *create_outbound_anim(AppDataPoint *data, ScrollDirection direction) {
  const int16_t to_dy = (direction == ScrollDirectionDown) ? -SCROLL_DIST_OUT : SCROLL_DIST_OUT;

  return create_anim_scroll_out(text_layer_get_layer(data->layer), SCROLL_DURATION, to_dy);
}

Animation *create_inbound_anim(AppDataPoint *data, ScrollDirection direction) {
  const int16_t from_dy = (direction == ScrollDirectionDown) ? -SCROLL_DIST_IN : SCROLL_DIST_IN;

  return create_anim_scroll_in(text_layer_get_layer(data->layer), SCROLL_DURATION, from_dy);
}

Animation *animation_for_scroll(AppDataPoint *data, ScrollDirection direction) {
  // sliding texts
  Animation *out_text = create_outbound_anim(data, direction);
  animation_set_handlers(out_text, (AnimationHandlers) {
    .stopped = after_scroll_swap_text,
}, data);
  Animation *in_text = create_inbound_anim(data, direction);

  return animation_sequence_create(out_text, in_text, NULL);
}

Animation *animation_for_bounce(AppDataPoint *data, ScrollDirection direction) {
  return create_inbound_anim(data, direction);
}

void ask_for_scroll(AppDataPoint *data, ScrollDirection direction) {
  Animation *scroll_animation;

  if (!data) {
    scroll_animation = animation_for_bounce(data, direction);
  } else {
    // data point switches immediately
    scroll_animation = animation_for_scroll(data, direction);
  }

  animation_unschedule(data->previous_animation);
  animation_schedule(scroll_animation);
  data->previous_animation = scroll_animation;
}

/*
void on_animation_stopped(Animation *anim, bool finished, void *context){
    #ifdef PBL_BW
        property_animation_destroy((PropertyAnimation*) anim);
    #endif
}

void animation_update_handler(struct Animation *animation, const AnimationProgress distance_normalized){
	int percent = (distance_normalized/(ANIMATION_NORMALIZED_MAX/100));
	if(animation_get_context(animation)){
		if(percent > 51){
			TextLayerUpdate *update = animation_get_context(animation);
			text_layer_set_text(update->layer, update->new_text);
		}
	}
}

const PropertyAnimationImplementation implementation = {
	.base = {
		.update = animation_update_handler
	}
};

void animate_layer(Layer *layer, GRect *start, GRect *finish, int duration, int delay){
    PropertyAnimation *anim = property_animation_create_layer_frame(layer, start, finish);

    animation_set_duration((Animation*) anim, duration);
    animation_set_delay((Animation*) anim, delay);

    AnimationHandlers handlers = {
        .stopped = (AnimationStoppedHandler) stopped
    };
    animation_set_handlers((Animation*) anim, handlers, NULL);

    animation_schedule((Animation*) anim);
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
*/
