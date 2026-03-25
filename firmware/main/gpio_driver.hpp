#pragma once

#include "driver/gpio.h"
#include "esp_err.h"

class GpioDriver {
public:
    GpioDriver(gpio_num_t pin, gpio_mode_t mode);
    virtual ~GpioDriver() = default;

    gpio_num_t get_pin() const { return pin_; }
    int get_level() const;
    esp_err_t set_mode(gpio_mode_t mode);

protected:
    gpio_num_t pin_;
};
