#include "gpio_driver.hpp"
#include "esp_log.h"

static const char* TAG = "gpio_driver";

GpioDriver::GpioDriver(gpio_num_t pin, gpio_mode_t mode) : pin_(pin) {
    gpio_config_t config = {
        .pin_bit_mask = 1ULL << pin,
        .mode = mode,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&config));
}

int GpioDriver::get_level() const {
    return gpio_get_level(pin_);
}

esp_err_t GpioDriver::set_mode(gpio_mode_t mode) {
    gpio_config_t config = {
        .pin_bit_mask = 1ULL << pin_,
        .mode = mode,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    return gpio_config(&config);
}
