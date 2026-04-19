#pragma once

#include "gpio_driver.hpp"

/**
 * @brief LED driver with on/off/toggle control
 */
class LedDriver : public GpioDriver {
  public:
	/**
	 * @brief Construct a LedDriver
	 * @param pin GPIO pin connected to LED (must be valid board GPIO)
	 * @note Construction cannot fail - pin is validated at compile time via board_config.h.
	 *       LED init is purely runtime GPIO configuration which ESP-IDF guarantees success.
	 */
	LedDriver (gpio_num_t pin);

	/**
	 * @brief Turn LED on
	 */
	void on ();

	/**
	 * @brief Turn LED off
	 */
	void off ();

	/**
	 * @brief Toggle LED state
	 */
	void toggle ();

	/**
	 * @brief Get current LED state
	 * @return true if on, false if off
	 */
	bool
	get_state () const {
		return state_;
	}

  private:
	bool state_ = false;
};
