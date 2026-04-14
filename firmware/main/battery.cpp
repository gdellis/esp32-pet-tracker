#include "battery.hpp"
#include "board_config.h"
#include "esp_log.h"

static const char* TAG = "battery";

esp_err_t
BatteryDriver::init () {
	adc_oneshot_unit_init_cfg_t init_config = {};
	init_config.unit_id = BATTERY_ADC_UNIT;

	esp_err_t err = adc_oneshot_new_unit (&init_config, &adc_handle_);
	if (err != ESP_OK) {
		ESP_LOGE (TAG, "Failed to initialize ADC unit: %s", esp_err_to_name (err));
		return err;
	}

	adc_oneshot_chan_cfg_t config = {};
	config.bitwidth = ADC_BITWIDTH_12;
	config.atten = ADC_ATTEN_DB_12;

	err = adc_oneshot_config_channel (adc_handle_, BATTERY_ADC_CHANNEL, &config);
	if (err != ESP_OK) {
		ESP_LOGE (TAG, "Failed to configure ADC channel: %s", esp_err_to_name (err));
		return err;
	}

	initialized_ = true;
	ESP_LOGI (TAG, "Battery driver initialized");
	return ESP_OK;
}

esp_err_t
BatteryDriver::deinit () {
	if (initialized_) {
		adc_oneshot_del_unit (adc_handle_);
		initialized_ = false;
	}
	return ESP_OK;
}

std::optional<uint16_t>
BatteryDriver::read_voltage_mv () {
	if (!initialized_) {
		ESP_LOGW (TAG, "Battery not initialized");
		return std::nullopt;
	}

	int raw = 0;
	esp_err_t err = adc_oneshot_read (adc_handle_, BATTERY_ADC_CHANNEL, &raw);
	if (err != ESP_OK) {
		ESP_LOGW (TAG, "ADC read failed: %s", esp_err_to_name (err));
		return std::nullopt;
	}

	uint32_t voltage_mv = (uint32_t)raw * BATTERY_ADC_VREF_MV * 2 / BATTERY_ADC_MAX;
	return (uint16_t)(voltage_mv);
}

std::optional<uint8_t>
BatteryDriver::read_percentage () {
	auto voltage_opt = read_voltage_mv ();
	if (!voltage_opt) {
		return std::nullopt;
	}

	uint16_t voltage = *voltage_opt;
	if (voltage >= BATTERY_FULL_SCALE_MV) {
		return 100;
	}
	if (voltage <= BATTERY_EMPTY_SCALE_MV) {
		return 0;
	}
	return (uint8_t)((voltage - BATTERY_EMPTY_SCALE_MV) * 100
					 / (BATTERY_FULL_SCALE_MV - BATTERY_EMPTY_SCALE_MV));
}
