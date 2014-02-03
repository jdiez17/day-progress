#include <pebble.h>

// 3.57 pixels per second

Window* window;
Layer* root_layer;
struct tm last_time;

void clear_screen(GContext* ctx) {
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_fill_rect(ctx, (GRect) {
            .origin = { .x = 0, .y = 0 },
            .size = { .h = 144, .w = 168}
    }, 0, GCornerNone);
    graphics_context_set_fill_color(ctx, GColorBlack);
}

void layer_update(Layer* l, GContext* ctx) {
    int seconds_so_far = last_time.tm_hour * 3600 + last_time.tm_min * 60 + last_time.tm_sec;
    clear_screen(ctx); // TODO: Optimize this

    // First: draw a rectangle that represents full lines so far
    int lines = seconds_so_far / 168;

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Seconds so far: %d, lines: %d", seconds_so_far, lines);
    graphics_fill_rect(ctx, (GRect) {
        .origin = { .x = 0, .y = 0 },
        .size = { .h = lines, .w = 168 }
    }, 0, GCornerNone);
}

void handle_tick(struct tm* t, TimeUnits delta_t) {
    last_time = *t; // copy to static variable

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Tick!");
    layer_mark_dirty(root_layer);
}

void init(void) {
  window = window_create();
  window_stack_push(window, true);

  root_layer = window_get_root_layer(window);
  layer_set_update_proc(root_layer, layer_update);

  tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
}

void deinit(void) {
  window_destroy(window);

  tick_timer_service_unsubscribe();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
