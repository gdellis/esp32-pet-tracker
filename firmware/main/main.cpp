/**
 * @file main.cpp
 * @brief ESP32 Pet Tracker application entry point
 *
 * Pet tracker firmware that:
 * - Acquires GPS location via UART
 * - Transmits location data over LoRa SX1262
 * - Supports deep sleep between transmissions
 * - Button press to toggle LED and trigger transmission
 */

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

#include "accelerometer.hpp"
#include "board_config.h"
#include "button_handler.hpp"
#include "deep_sleep.hpp"
#include "gpio_driver.hpp"
#include "gps.hpp"
#include "led_driver.hpp"
#include "lora/sx1262.hpp"
#include "state_machine.hpp"

static const char* TAG = "pet-tracker";

#define SLEEP_TIMEOUT_MS	   30000
#define DEEP_SLEEP_DURATION_US (10LL * 1000000)

void
app_main (void) {
	ESP_LOGI (TAG, "Starting pet tracker...");

	// Initialize hardware components
	gpio_driver::init ();
	led_driver::init ();
	button_handler::init ();

	// Initialize GPS (UART1: TX=GPIO7, RX=GPIO15 @ 115200 baud)
	gps::init (UART_NUM_1);

	// Initialize LoRa (SPI2: MOSI=GPIO4, MISO=GPIO5, SCLK=GPIO6, CS=GPIO10)
	lora_driver::init ();

	// Initialize accelerometer (I2C0: SDA=GPIO2, SCL=GPIO3, INT=GPIO9)
	accelerometer::init (I2C_NUM_0, BOARD_ACCEL_INT_PIN);
	accelerometer::enable_motion_interrupt (2000); // 200mg threshold

	// Create and initialize state machine
	static Gps gps;
	static LoRaDriver lora (spi_host_device_t::SPI2_HOST, GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_6,
							GPIO_NUM_10, GPIO_NUM_0, GPIO_NUM_0, GPIO_NUM_9);
	static Accelerometer accel (I2C_NUM_0, BOARD_ACCEL_INT_PIN);

	static TrackerStateMachine state_machine (gps, lora, accel);
	state_machine.init ();

	ESP_LOGI (TAG, "Pet tracker initialized, entering main loop");

	while (true) {
		state_machine.run ();
		vTaskDelay (pdMS_TO_TICKS (1000));
	}
}
