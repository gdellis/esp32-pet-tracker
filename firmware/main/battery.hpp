#pragma once

#include "esp_adc/adc_oneshot.h"
#include "esp_err.h"
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
	BatteryDriver ();

	esp_err_t init ();

	uint16_t
	read_voltage_mv () {
		int raw = 0;
		esp_err_t err = adc_oneshot_read (adc_handle_, BATTERY_ADC_CHANNEL, &raw);
		if (err != ESP_OK) {
			return 0;
		}

		float voltage_ratio
			= (float)(BATTERY_DIVIDER_R1_OHMS + BATTERY_DIVIDER_R2_OHMS) / BATTERY_DIVIDER_R2_OHMS;
		float input_voltage = (raw * (float)BATTERY_ADC_VREF_MV / BATTERY_ADC_MAX) * voltage_ratio;
		return (uint16_t)(input_voltage + 0.5f);
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
