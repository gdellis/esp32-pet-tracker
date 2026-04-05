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

extern "C" void
app_main (void) {
	ESP_LOGI (TAG, "Starting pet tracker...");

	static LedDriver led (BOARD_LED_PIN);

	static Gps gps (UART_NUM_1);
	gps.init ();

	static LoRaDriver lora (spi_host_device_t::SPI2_HOST, BOARD_LORA_MOSI_PIN, BOARD_LORA_MISO_PIN,
							BOARD_LORA_SCK_PIN, BOARD_LORA_NSS_PIN, BOARD_LORA_RESET_PIN,
							BOARD_LORA_BUSY_PIN, BOARD_LORA_DIO1_PIN);
	lora.init ();

	static Accelerometer accel (I2C_NUM_0, BOARD_ACCEL_INT_PIN);
	accel.init ();
	accel.enable_motion_interrupt (2000);

	static BleServer ble;
	ble.init ();
	ble.start ();

	static TrackerStateMachine state_machine (gps, lora, accel, ble, led);
	state_machine.init ();

	ESP_LOGI (TAG, "Pet tracker initialized, entering main loop");

	while (true) {
		state_machine.run ();
		vTaskDelay (pdMS_TO_TICKS (1000));
	}
}
