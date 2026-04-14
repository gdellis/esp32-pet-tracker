#pragma once

#include "esp_adc/adc_oneshot.h"
#include "esp_err.h"
#include "esp_log.h"
#include <cstdint>
#include <optional>

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

	/// @return Battery voltage in millivolts, or std::nullopt if read failed
	std::optional<uint16_t> read_voltage_mv ();
	/// @return Battery percentage (0-100), or std::nullopt if read failed
	std::optional<uint8_t> read_percentage ();

  private:
	adc_oneshot_unit_handle_t adc_handle_;
	bool initialized_;
};
