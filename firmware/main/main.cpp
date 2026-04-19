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
#include "esp_task_wdt.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

#include "accelerometer.hpp"
#include "battery.hpp"
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
	esp_err_t gps_err = gps.init ();
	if (gps_err != ESP_OK) {
		ESP_LOGE (TAG, "GPS init failed: %s", esp_err_to_name (gps_err));
	}

	static LoRaDriver lora (spi_host_device_t::SPI2_HOST, BOARD_LORA_MOSI_PIN, BOARD_LORA_MISO_PIN,
							BOARD_LORA_SCK_PIN, BOARD_LORA_NSS_PIN, BOARD_LORA_RESET_PIN,
							BOARD_LORA_BUSY_PIN, BOARD_LORA_DIO1_PIN);
	ESP_ERROR_CHECK (lora.init ());

	static Accelerometer accel (I2C_NUM_0, BOARD_ACCEL_INT_PIN);
	esp_err_t accel_err = accel.init ();
	if (accel_err != ESP_OK) {
		ESP_LOGE (TAG, "Accelerometer init failed: %s", esp_err_to_name (accel_err));
	}
	ESP_ERROR_CHECK (accel.enable_motion_interrupt (2000));

	static BleServer ble;
	ESP_ERROR_CHECK (ble.init ());
	ESP_ERROR_CHECK (ble.start ());

	static BatteryDriver battery;
	esp_err_t battery_err = battery.init ();
	if (battery_err != ESP_OK) {
		ESP_LOGE (TAG, "Battery init failed: %s", esp_err_to_name (battery_err));
	}

	static TrackerStateMachine state_machine (gps, lora, accel, ble, led, battery);
	esp_err_t sm_err = state_machine.init ();
	if (sm_err != ESP_OK) {
		ESP_LOGE (TAG, "State machine init failed: %s", esp_err_to_name (sm_err));
	}

	ESP_LOGI (TAG, "Pet tracker initialized, entering main loop");

	esp_task_wdt_config_t wdt_config = {
		.timeout_ms = BOARD_WDT_TIMEOUT_MS,
		.idle_core_mask = 0,  /* Both cores - app is single-threaded */
		.trigger_panic = true,
	};
	esp_task_wdt_init (&wdt_config);
	esp_task_wdt_add (NULL);

	ESP_LOGI (TAG, "Watchdog initialized with %ums timeout", BOARD_WDT_TIMEOUT_MS);

	while (true) {
		esp_task_wdt_reset ();
		state_machine.run ();
		vTaskDelay (pdMS_TO_TICKS (1000));
	}
}
