#pragma once

#include "gpio_driver.hpp"

class LedDriver : public GpioDriver {
public:
    LedDriver(gpio_num_t pin);
    void on();
    void off();
    void toggle();
    bool get_state() const { return state_; }

private:
    bool state_ = false;
};
