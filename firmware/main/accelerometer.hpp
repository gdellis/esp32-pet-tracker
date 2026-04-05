#pragma once

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_sleep.h"
#include <stdint.h>

#define LIS3DH_I2C_ADDR 0x19

#define LIS3DH_REG_STATUS_REG	 0x27
#define LIS3DH_REG_CTRL_REG1	 0x20
#define LIS3DH_REG_CTRL_REG2	 0x21
#define LIS3DH_REG_CTRL_REG3	 0x22
#define LIS3DH_REG_CTRL_REG4	 0x23
#define LIS3DH_REG_CTRL_REG5	 0x24
#define LIS3DH_REG_INT1_CFG		 0x30
#define LIS3DH_REG_INT1_SRC		 0x31
#define LIS3DH_REG_INT1_THS		 0x32
#define LIS3DH_REG_INT1_DURATION 0x33

#define LIS3DH_CTRL_REG1_XEN	  0x01
#define LIS3DH_CTRL_REG1_YEN	  0x02
#define LIS3DH_CTRL_REG1_ZEN	  0x04
#define LIS3DH_CTRL_REG1_LPEN	  0x08
#define LIS3DH_CTRL_REG1_ODR_MASK 0xF0

#define LIS3DH_CTRL_REG3_I1_INT1 0x40

#define LIS3DH_CTRL_REG4_BDU	   0x80
#define LIS3DH_CTRL_REG4_HR		   0x08
#define LIS3DH_CTRL_REG4_SCALE_2G  0x00
#define LIS3DH_CTRL_REG4_SCALE_4G  0x10
#define LIS3DH_CTRL_REG4_SCALE_8G  0x20
#define LIS3DH_CTRL_REG4_SCALE_16G 0x30

#define LIS3DH_CTRL_REG5_LIR_INT1 0x08

#define LIS3DH_INT_CFG_6D	0x01
#define LIS3DH_INT_CFG_4D	0x02
#define LIS3DH_INT_CFG_ZHIE 0x20
#define LIS3DH_INT_CFG_ZLIE 0x10
#define LIS3DH_INT_CFG_YHIE 0x08
#define LIS3DH_INT_CFG_YLIE 0x04
#define LIS3DH_INT_CFG_XHIE 0x02
#define LIS3DH_INT_CFG_XLIE 0x01

class Accelerometer {
  public:
	Accelerometer (i2c_port_t i2c_port, gpio_num_t int_pin);

	static esp_err_t init (i2c_port_t i2c_port, gpio_num_t int_pin);
	esp_err_t init ();
	esp_err_t enable_motion_interrupt (uint16_t threshold_mg);
	esp_err_t clear_interrupt ();
	bool has_motion ();
	esp_err_t read_axis (int16_t& x, int16_t& y, int16_t& z);

	esp_err_t enable_wakeup (gpio_num_t gpio_num);
	bool is_wakeup_source ();

  private:
	esp_err_t write_reg (uint8_t reg, uint8_t value);
	esp_err_t read_reg (uint8_t reg, uint8_t& value);
	esp_err_t read_reg16 (uint8_t reg, int16_t& value);

	i2c_port_t i2c_port_;
	gpio_num_t int_pin_;
};
