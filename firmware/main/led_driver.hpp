#pragma once

#include "gpio_driver.hpp"

/**
 * @brief LED driver with on/off/toggle control
 */
class LedDriver : public GpioDriver {
public:
    /**
     * @brief Construct a LedDriver
     * @param pin GPIO pin connected to LED
     */
    LedDriver(gpio_num_t pin);

    /**
     * @brief Turn LED on
     */
    void on();

    /**
     * @brief Turn LED off
     */
    void off();

    /**
     * @brief Toggle LED state
     */
    void toggle();

    /**
     * @brief Get current LED state
     * @return true if on, false if off
     */
    bool get_state() const { return state_; }

private:
    bool state_ = false;
};
