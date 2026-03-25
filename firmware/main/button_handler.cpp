#include "button_handler.hpp"

ButtonHandler::ButtonHandler(gpio_num_t pin, int64_t debounce_us)
    : pin_(pin), debounce_us_(debounce_us), last_press_time_(0) {}

bool ButtonHandler::check_pressed() {
    int64_t now = esp_timer_get_time();
    int level = gpio_get_level(pin_);

    if (level == 0 && (now - last_press_time_) > debounce_us_) {
        last_press_time_ = now;
        return true;
    }
    return false;
}
