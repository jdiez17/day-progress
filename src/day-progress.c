#include <pebble.h>

Window* window;

void init(void) {
  window = window_create();
  window_stack_push(window, true);
}

void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
