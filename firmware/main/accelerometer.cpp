#include "accelerometer.hpp"
#include "board_config.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "freertos/FreeRTOS.h"

static const char* TAG = "lis3dh";

static i2c_port_t s_i2c_port = I2C_NUM_0;
static gpio_num_t s_int_pin = GPIO_NUM_0;
static i2c_master_bus_handle_t s_i2c_bus_handle;
static i2c_master_dev_handle_t s_i2c_device_handle;

Accelerometer::Accelerometer (i2c_port_t i2c_port, gpio_num_t int_pin)
	: i2c_port_ (i2c_port), int_pin_ (int_pin) {}

esp_err_t
Accelerometer::init (i2c_port_t i2c_port, gpio_num_t int_pin) {
	s_i2c_port = i2c_port;
	s_int_pin = int_pin;

	i2c_master_bus_config_t bus_conf = {
		.i2c_port = i2c_port,
		.sda_io_num = BOARD_ACCEL_SDA_PIN,
		.scl_io_num = BOARD_ACCEL_SCL_PIN,
		.clk_source = I2C_CLK_SRC_DEFAULT,
		.glitch_ignore_cnt = 7,
		.intr_priority = 0,
		.trans_queue_depth = 0,
		.flags = {
			.enable_internal_pullup = true,
			.allow_pd = false,
		},
	};

	esp_err_t ret = i2c_new_master_bus (&bus_conf, &s_i2c_bus_handle);
	if (ret != ESP_OK) {
		return ret;
	}

	i2c_device_config_t dev_conf = {
		.dev_addr_length = I2C_ADDR_BIT_LEN_7,
		.device_address = LIS3DH_I2C_ADDR,
		.scl_speed_hz = I2C_CLOCK_SPEED_HZ,
		.scl_wait_us = 0,
		.flags = {
			.disable_ack_check = false,
		},
	};

	ret = i2c_master_bus_add_device (s_i2c_bus_handle, &dev_conf, &s_i2c_device_handle);
	if (ret != ESP_OK) {
		return ret;
	}

	return ESP_OK;
}

esp_err_t
Accelerometer::init () {
	uint8_t whoami = 0;
	esp_err_t ret = read_reg (0x0F, whoami);
	if (ret != ESP_OK) {
		ESP_LOGE (TAG, "Failed to read WHOAMI register");
		return ret;
	}

	if (whoami != 0x33) {
		ESP_LOGE (TAG, "Unexpected WHOAMI: 0x%02x (expected 0x33)", whoami);
		return ESP_ERR_NOT_FOUND;
	}

	ret = write_reg (LIS3DH_REG_CTRL_REG1, 0x00);
	if (ret != ESP_OK) {
		return ret;
	}

	ret = write_reg (LIS3DH_REG_CTRL_REG1,
					 LIS3DH_CTRL_REG1_XEN | LIS3DH_CTRL_REG1_YEN | LIS3DH_CTRL_REG1_ZEN | 0x07);
	if (ret != ESP_OK) {
		return ret;
	}

	ret = write_reg (LIS3DH_REG_CTRL_REG2, 0x00);
	if (ret != ESP_OK) {
		return ret;
	}

	ret = write_reg (LIS3DH_REG_CTRL_REG3, 0x00);
	if (ret != ESP_OK) {
		return ret;
	}

	ret = write_reg (LIS3DH_REG_CTRL_REG4,
					 LIS3DH_CTRL_REG4_BDU | LIS3DH_CTRL_REG4_HR | LIS3DH_CTRL_REG4_SCALE_2G);
	if (ret != ESP_OK) {
		return ret;
	}

	ret = write_reg (LIS3DH_REG_CTRL_REG5, 0x00);
	if (ret != ESP_OK) {
		return ret;
	}

	gpio_config_t io_conf = { .pin_bit_mask = (1ULL << int_pin_),
							  .mode = GPIO_MODE_INPUT,
							  .pull_up_en = GPIO_PULLUP_DISABLE,
							  .pull_down_en = GPIO_PULLDOWN_DISABLE,
							  .intr_type = GPIO_INTR_DISABLE };

	ret = gpio_config (&io_conf);
	if (ret != ESP_OK) {
		return ret;
	}

	ESP_LOGI (TAG, "LIS3DH initialized");
	return ESP_OK;
}

esp_err_t
Accelerometer::enable_motion_interrupt (uint16_t threshold_mg) {
	uint8_t odr = 0x07;
	esp_err_t ret = write_reg (LIS3DH_REG_CTRL_REG1, LIS3DH_CTRL_REG1_XEN | LIS3DH_CTRL_REG1_YEN
														 | LIS3DH_CTRL_REG1_ZEN | odr);
	if (ret != ESP_OK) {
		return ret;
	}

	ret = write_reg (LIS3DH_REG_CTRL_REG3, LIS3DH_CTRL_REG3_I1_INT1);
	if (ret != ESP_OK) {
		return ret;
	}

	uint8_t threshold = ((uint32_t)threshold_mg * 128) / 1000;
	if (threshold > 127) {
		threshold = 127;
	}

	ret = write_reg (LIS3DH_REG_INT1_THS, threshold);
	if (ret != ESP_OK) {
		return ret;
	}

	ret = write_reg (LIS3DH_REG_INT1_DURATION, 0x00);
	if (ret != ESP_OK) {
		return ret;
	}

	ret = write_reg (LIS3DH_REG_INT1_CFG,
					 LIS3DH_INT_CFG_XHIE | LIS3DH_INT_CFG_YHIE | LIS3DH_INT_CFG_ZHIE);
	if (ret != ESP_OK) {
		return ret;
	}

	ret = write_reg (LIS3DH_REG_CTRL_REG5, LIS3DH_CTRL_REG5_LIR_INT1);
	if (ret != ESP_OK) {
		return ret;
	}

	ESP_LOGI (TAG, "Motion interrupt enabled, threshold=%u mg (reg=0x%02x)", threshold_mg,
			  threshold);
	return ESP_OK;
}

esp_err_t
Accelerometer::clear_interrupt () {
	uint8_t src = 0;
	return read_reg (LIS3DH_REG_INT1_SRC, src);
}

bool
Accelerometer::has_motion () {
	return gpio_get_level (int_pin_) == 1;
}

esp_err_t
Accelerometer::read_axis (int16_t& x, int16_t& y, int16_t& z) {
	esp_err_t ret = read_reg16 (0x28, x);
	if (ret != ESP_OK) {
		return ret;
	}

	ret = read_reg16 (0x2A, y);
	if (ret != ESP_OK) {
		return ret;
	}

	ret = read_reg16 (0x2C, z);
	if (ret != ESP_OK) {
		return ret;
	}

	return ESP_OK;
}

esp_err_t
Accelerometer::enable_wakeup (gpio_num_t gpio_num) {
	gpio_config_t io_conf = { .pin_bit_mask = 1ULL << gpio_num,
							  .mode = GPIO_MODE_INPUT,
							  .pull_up_en = GPIO_PULLUP_DISABLE,
							  .pull_down_en = GPIO_PULLDOWN_DISABLE,
							  .intr_type = GPIO_INTR_DISABLE };

	esp_err_t ret = gpio_config (&io_conf);
	if (ret != ESP_OK) {
		return ret;
	}

	ret = gpio_wakeup_enable (gpio_num, GPIO_INTR_HIGH_LEVEL);
	if (ret != ESP_OK) {
		return ret;
	}

	return esp_sleep_enable_gpio_wakeup ();
}

bool
Accelerometer::is_wakeup_source () {
	return (esp_sleep_get_wakeup_causes () & ESP_SLEEP_WAKEUP_GPIO) != 0;
}

static bool
is_valid_register (uint8_t reg) {
	return (reg >= LIS3DH_REG_CTRL_MIN && reg <= LIS3DH_REG_CTRL_MAX)
		   || (reg >= LIS3DH_REG_INT_MIN && reg <= LIS3DH_REG_INT_MAX) || reg == LIS3DH_REG_WHOAMI
		   || reg == LIS3DH_REG_STATUS || (reg >= LIS3DH_REG_OUT_XL && reg <= LIS3DH_REG_OUT_ZH);
}

esp_err_t
Accelerometer::write_reg (uint8_t reg, uint8_t value) {
	if (!is_valid_register (reg)) {
		ESP_LOGE (TAG, "Invalid register address: 0x%02x", reg);
		return ESP_ERR_INVALID_ARG;
	}
	uint8_t data[2] = { reg, value };

	return i2c_master_transmit (s_i2c_device_handle, data, 2, pdMS_TO_TICKS (100));
}

esp_err_t
Accelerometer::read_reg (uint8_t reg, uint8_t& value) {
	if (!is_valid_register (reg)) {
		ESP_LOGE (TAG, "Invalid register address: 0x%02x", reg);
		return ESP_ERR_INVALID_ARG;
	}
	esp_err_t ret = i2c_master_transmit (s_i2c_device_handle, &reg, 1, pdMS_TO_TICKS (100));
	if (ret != ESP_OK) {
		return ret;
	}

	return i2c_master_receive (s_i2c_device_handle, &value, 1, pdMS_TO_TICKS (100));
}

esp_err_t
Accelerometer::read_reg16 (uint8_t reg, int16_t& value) {
	uint8_t low = 0;
	uint8_t high = 0;

	esp_err_t ret = read_reg (reg, low);
	if (ret != ESP_OK) {
		return ret;
	}

	ret = read_reg (reg + 1, high);
	if (ret != ESP_OK) {
		return ret;
	}

	value = static_cast<int16_t> ((high << 8) | low);
	return ESP_OK;
}
