#include "button_handler.hpp"
#include <climits>

ButtonHandler::ButtonHandler(gpio_num_t pin, int64_t debounce_us)
    : pin_(pin), debounce_us_(debounce_us), last_press_time_(-INT64_MAX) {}

bool ButtonHandler::check_pressed() {
    int64_t now = esp_timer_get_time();
    int level = gpio_get_level(pin_);

    if (level == 0 && (now - last_press_time_) > debounce_us_) {
        last_press_time_ = now;
        return true;
    }
    return false;
}
