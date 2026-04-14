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
