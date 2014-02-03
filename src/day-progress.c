#include <pebble.h>

// 86400 / (144 * 168)
#define SECFGDS_PER_PIXEL 3.57 

// Change FG to GColorBlack to invert the watchface
#define FG  GColorWhite 
#define BG  (FG == GColorBlack ? GColorWhite : GColorBlack)

Window* window;
Layer* root_layer;
struct tm last_time;

int last_pixels = -1;

void clear_screen(GContext* ctx) {
    graphics_context_set_fill_color(ctx, BG);

    graphics_fill_rect(ctx, (GRect) {
        .origin = GPointZero,
        .size = GSize(144, 168)
    }, 0, GCornerNone);

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Cleared screen");
}

void layer_update(Layer* l, GContext* ctx) {
    int seconds_so_far = last_time.tm_hour * 3600 + last_time.tm_min * 60 + last_time.tm_sec;
    int pixels_so_far = seconds_so_far / SECFGDS_PER_PIXEL;

    if(last_pixels > pixels_so_far || last_pixels == -1)
        clear_screen(ctx); // This happens once every day 
    else if(last_pixels == pixels_so_far)
        return; // Nothing to do

    last_pixels = pixels_so_far;

    graphics_context_set_fill_color(ctx, FG);
    graphics_context_set_stroke_color(ctx, FG);

    // First: draw a rectangle that represents full lines so far
    int lines = (pixels_so_far / 144);

    graphics_fill_rect(ctx, (GRect) {
        .origin = GPointZero,
        .size = GSize(144, lines)
    }, 0, GCornerNone);

    // Second: draw a line from the beginning of the next line that represents seconds
    int stray_pixels = pixels_so_far % 144;
    
    graphics_draw_line(ctx, GPoint(0, lines), GPoint(stray_pixels, lines));

    // Last: log info
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Seconds so far: %d (%d pixels), lines: %d, stray pixels: %d", seconds_so_far, pixels_so_far, lines, stray_pixels);
}

void handle_tick(struct tm* t, TimeUnits delta_t) {
    last_time = *t; // copy to static variable
    layer_mark_dirty(root_layer);
}

void init(void) {
    window = window_create();
    window_stack_push(window, true);

    root_layer = window_get_root_layer(window);
    layer_set_update_proc(root_layer, layer_update);

    tick_timer_service_subscribe(SECOND_UNIT, handle_tick);

    time_t now = time(NULL);
    last_time = *localtime(&now); 
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
