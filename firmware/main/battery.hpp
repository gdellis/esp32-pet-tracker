#pragma once

#include "esp_adc/adc_oneshot.h"
#include "esp_err.h"
#include "esp_log.h"
#include <stdint.h>

constexpr adc_unit_t BATTERY_ADC_UNIT = ADC_UNIT_1;
constexpr adc_channel_t BATTERY_ADC_CHANNEL = ADC_CHANNEL_0;
constexpr uint32_t BATTERY_DIVIDER_R1_OHMS = 100000;
constexpr uint32_t BATTERY_DIVIDER_R2_OHMS = 100000;
constexpr uint32_t BATTERY_ADC_MAX = 4095;
constexpr uint32_t BATTERY_ADC_VREF_MV = 1100;
constexpr uint32_t BATTERY_FULL_SCALE_MV = 4200;
constexpr uint32_t BATTERY_EMPTY_SCALE_MV = 3000;

class BatteryDriver {
  public:
	BatteryDriver () : initialized_ (false) {}

	esp_err_t init ();
	esp_err_t deinit ();

	uint16_t
	read_voltage_mv () {
		if (!initialized_) {
			ESP_LOGW ("BATTERY", "Battery not initialized");
			return 0;
		}

		int raw = 0;
		esp_err_t err = adc_oneshot_read (adc_handle_, BATTERY_ADC_CHANNEL, &raw);
		if (err != ESP_OK) {
			ESP_LOGW ("BATTERY", "ADC read failed: %s", esp_err_to_name (err));
			return 0;
		}

		uint32_t voltage_mv = (uint32_t)raw * BATTERY_ADC_VREF_MV * 2 / BATTERY_ADC_MAX;
		return (uint16_t)(voltage_mv);
	}

	uint8_t
	read_percentage () {
		uint16_t voltage = read_voltage_mv ();
		if (voltage >= BATTERY_FULL_SCALE_MV) {
			return 100;
		}
		if (voltage <= BATTERY_EMPTY_SCALE_MV) {
			return 0;
		}
		return (uint8_t)((voltage - BATTERY_EMPTY_SCALE_MV) * 100
						 / (BATTERY_FULL_SCALE_MV - BATTERY_EMPTY_SCALE_MV));
	}

  private:
	adc_oneshot_unit_handle_t adc_handle_;
	bool initialized_;
};
