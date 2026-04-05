#pragma once

#include "driver/gpio.h"
#include "esp_err.h"

/**
 * @brief GPIO driver for pin access and configuration
 */
class GpioDriver {
  public:
	/**
	 * @brief Construct a GpioDriver
	 * @param pin GPIO pin number
	 * @param mode Initial GPIO mode
	 */
	GpioDriver (gpio_num_t pin, gpio_mode_t mode);

	/**
	 * @brief Destructor
	 */
	virtual ~GpioDriver () = default;

	/**
	 * @brief Get the GPIO pin number
	 * @return GPIO pin number
	 */
	gpio_num_t
	get_pin () const {
		return pin_;
	}

	/**
	 * @brief Get current GPIO input level
	 * @return Current pin level (0 or 1)
	 */
	int get_level () const;

	/**
	 * @brief Set GPIO mode
	 * @param mode New GPIO mode (input, output, etc.)
	 * @return ESP_OK on success, error code otherwise
	 */
	esp_err_t set_mode (gpio_mode_t mode);

  protected:
	gpio_num_t pin_;
};
