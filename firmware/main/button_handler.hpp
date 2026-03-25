#pragma once

#include "driver/gpio.h"
#include "esp_timer.h"

class ButtonHandler {
public:
    ButtonHandler(gpio_num_t pin, int64_t debounce_us = 200000);
    bool check_pressed();

private:
    gpio_num_t pin_;
    int64_t debounce_us_;
    int64_t last_press_time_;
};
