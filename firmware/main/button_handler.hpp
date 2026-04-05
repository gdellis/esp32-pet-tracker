#pragma once

#include "driver/gpio.h"
#include "esp_timer.h"

/**
 * @brief Button handler with software debouncing
 */
class ButtonHandler {
  public:
	/**
	 * @brief Construct a ButtonHandler
	 * @param pin GPIO pin connected to button
	 * @param debounce_us Debounce time in microseconds (default 200ms)
	 */
	ButtonHandler (gpio_num_t pin, int64_t debounce_us = 200000);

	/**
	 * @brief Check if button was pressed (with debouncing)
	 * @return true if button press detected, false otherwise
	 */
	bool check_pressed ();

  private:
	gpio_num_t pin_;
	int64_t debounce_us_;
	int64_t last_press_time_;
};
