#include "led_driver.hpp"
#include "esp_log.h"

static const char* TAG = "led_driver";

LedDriver::LedDriver(gpio_num_t pin) : GpioDriver(pin, GPIO_MODE_OUTPUT) {
    ESP_LOGD(TAG, "LED driver initialized on GPIO %d", pin);
}

void LedDriver::on() {
    ESP_ERROR_CHECK(gpio_set_level(pin_, 1));
    state_ = true;
}

void LedDriver::off() {
    ESP_ERROR_CHECK(gpio_set_level(pin_, 0));
    state_ = false;
}

void LedDriver::toggle() {
    int level = gpio_get_level(pin_);
    ESP_ERROR_CHECK(gpio_set_level(pin_, !level));
    state_ = !level;
}
