#pragma once

#include "esp_err.h"
#include "nvs.h"
#include "nvs_flash.h"
#include <stdint.h>
#include <string.h>

struct TrackerConfig {
	uint32_t device_id;
	uint32_t sleep_interval_ms;
	uint32_t stationary_interval_ms;
	uint8_t tx_power;
	uint8_t spreading_factor;
	char device_name[32];
};

class Config {
  public:
	static esp_err_t init ();

	static esp_err_t load (TrackerConfig& config);
	static esp_err_t save (const TrackerConfig& config);

	static esp_err_t get_device_id (uint32_t& device_id);
	static esp_err_t set_device_id (uint32_t device_id);

	static esp_err_t get_sleep_interval (uint32_t& interval_ms);
	static esp_err_t set_sleep_interval (uint32_t interval_ms);

	static esp_err_t get_stationary_interval (uint32_t& interval_ms);
	static esp_err_t set_stationary_interval (uint32_t interval_ms);

	static esp_err_t get_tx_power (uint8_t& tx_power);
	static esp_err_t set_tx_power (uint8_t tx_power);

	static esp_err_t get_spreading_factor (uint8_t& sf);
	static esp_err_t set_spreading_factor (uint8_t sf);

	static esp_err_t get_device_name (char* name, size_t max_len);
	static esp_err_t set_device_name (const char* name);

  private:
	static esp_err_t get_u32 (nvs_handle_t handle, const char* key, uint32_t& value);
	static esp_err_t set_u32 (nvs_handle_t handle, const char* key, uint32_t value);
	static esp_err_t get_u8 (nvs_handle_t handle, const char* key, uint8_t& value);
	static esp_err_t set_u8 (nvs_handle_t handle, const char* key, uint8_t value);
	static esp_err_t get_str (nvs_handle_t handle, const char* key, char* value, size_t max_len);
	static esp_err_t set_str (nvs_handle_t handle, const char* key, const char* value);

	static constexpr char NVS_NS[] = "tracker_config";
	static constexpr uint32_t DEFAULT_DEVICE_ID = 0x12345678;
	static constexpr uint32_t DEFAULT_SLEEP_INTERVAL_MS = 300000;
	static constexpr uint32_t DEFAULT_STATIONARY_INTERVAL_MS = 600000;
	static constexpr uint8_t DEFAULT_TX_POWER = 22;
	static constexpr uint8_t DEFAULT_SPREADING_FACTOR = 7;
	static constexpr const char* DEFAULT_DEVICE_NAME = "PetTracker";
};
