#include "config.hpp"
#include "esp_log.h"

static const char* TAG = "config";

esp_err_t
Config::init () {
	esp_err_t ret = nvs_flash_init ();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_LOGW (TAG, "NVS partition was truncated, erasing and reinitializing");
		ret = nvs_flash_erase ();
		if (ret != ESP_OK) {
			ESP_LOGE (TAG, "Failed to erase NVS: %s", esp_err_to_name (ret));
			return ret;
		}
		ret = nvs_flash_init ();
	}
	return ret;
}

esp_err_t
Config::load (TrackerConfig& config) {
	nvs_handle_t handle;
	esp_err_t ret = nvs_open (NVS_NS, NVS_READONLY, &handle);
	if (ret != ESP_OK) {
		ESP_LOGE (TAG, "Failed to open NVS namespace: %s", esp_err_to_name (ret));
		return ret;
	}

	bool any_read_ok = false;

	ret = get_u32 (handle, "device_id", config.device_id);
	if (ret == ESP_OK) {
		any_read_ok = true;
	} else {
		config.device_id = DEFAULT_DEVICE_ID;
	}

	ret = get_u32 (handle, "sleep_interval", config.sleep_interval_ms);
	if (ret == ESP_OK) {
		any_read_ok = true;
	} else {
		config.sleep_interval_ms = DEFAULT_SLEEP_INTERVAL_MS;
	}

	ret = get_u32 (handle, "stationary_interval", config.stationary_interval_ms);
	if (ret == ESP_OK) {
		any_read_ok = true;
	} else {
		config.stationary_interval_ms = DEFAULT_STATIONARY_INTERVAL_MS;
	}

	ret = get_u8 (handle, "tx_power", config.tx_power);
	if (ret == ESP_OK) {
		any_read_ok = true;
	} else {
		config.tx_power = DEFAULT_TX_POWER;
	}

	ret = get_u8 (handle, "spreading_factor", config.spreading_factor);
	if (ret == ESP_OK) {
		any_read_ok = true;
	} else {
		config.spreading_factor = DEFAULT_SPREADING_FACTOR;
	}

	ret = get_str (handle, "device_name", config.device_name, sizeof (config.device_name));
	if (ret == ESP_OK) {
		any_read_ok = true;
	} else {
		strlcpy (config.device_name, DEFAULT_DEVICE_NAME, sizeof (config.device_name));
	}

	nvs_close (handle);
	if (any_read_ok) {
		ESP_LOGI (
			TAG, "Config loaded: device_id=0x%08x, sleep=%ums, stationary=%ums, tx_power=%u, sf=%u",
			config.device_id, config.sleep_interval_ms, config.stationary_interval_ms,
			config.tx_power, config.spreading_factor);
	} else {
		ESP_LOGW (TAG,
				  "Config load failed, using defaults: device_id=0x%08x, sleep=%ums, "
				  "stationary=%ums, tx_power=%u, sf=%u",
				  config.device_id, config.sleep_interval_ms, config.stationary_interval_ms,
				  config.tx_power, config.spreading_factor);
	}
	return any_read_ok ? ESP_OK : ESP_ERR_NOT_FOUND;
}

esp_err_t
Config::save (const TrackerConfig& config) {
	nvs_handle_t handle;
	esp_err_t ret = nvs_open (NVS_NS, NVS_READWRITE, &handle);
	if (ret != ESP_OK) {
		ESP_LOGE (TAG, "Failed to open NVS namespace: %s", esp_err_to_name (ret));
		return ret;
	}

	ret = set_u32 (handle, "device_id", config.device_id);
	if (ret != ESP_OK) {
		goto cleanup;
	}

	ret = set_u32 (handle, "sleep_interval", config.sleep_interval_ms);
	if (ret != ESP_OK) {
		goto cleanup;
	}

	ret = set_u32 (handle, "stationary_interval", config.stationary_interval_ms);
	if (ret != ESP_OK) {
		goto cleanup;
	}

	ret = set_u8 (handle, "tx_power", config.tx_power);
	if (ret != ESP_OK) {
		goto cleanup;
	}

	ret = set_u8 (handle, "spreading_factor", config.spreading_factor);
	if (ret != ESP_OK) {
		goto cleanup;
	}

	ret = set_str (handle, "device_name", config.device_name);
	if (ret != ESP_OK) {
		goto cleanup;
	}

	ret = nvs_commit (handle);
	if (ret != ESP_OK) {
		ESP_LOGE (TAG, "Failed to commit NVS: %s", esp_err_to_name (ret));
	} else {
		ESP_LOGI (TAG, "Config saved successfully");
	}

cleanup:
	nvs_close (handle);
	return ret;
}

esp_err_t
Config::get_device_id (uint32_t& device_id) {
	nvs_handle_t handle;
	esp_err_t ret = nvs_open (NVS_NS, NVS_READONLY, &handle);
	if (ret != ESP_OK) {
		return ret;
	}
	ret = get_u32 (handle, "device_id", device_id);
	nvs_close (handle);
	return ret;
}

esp_err_t
Config::set_device_id (uint32_t device_id) {
	nvs_handle_t handle;
	esp_err_t ret = nvs_open (NVS_NS, NVS_READWRITE, &handle);
	if (ret != ESP_OK) {
		return ret;
	}
	ret = set_u32 (handle, "device_id", device_id);
	nvs_close (handle);
	return ret;
}

esp_err_t
Config::get_sleep_interval (uint32_t& interval_ms) {
	nvs_handle_t handle;
	esp_err_t ret = nvs_open (NVS_NS, NVS_READONLY, &handle);
	if (ret != ESP_OK) {
		return ret;
	}
	ret = get_u32 (handle, "sleep_interval", interval_ms);
	nvs_close (handle);
	return ret;
}

esp_err_t
Config::set_sleep_interval (uint32_t interval_ms) {
	nvs_handle_t handle;
	esp_err_t ret = nvs_open (NVS_NS, NVS_READWRITE, &handle);
	if (ret != ESP_OK) {
		return ret;
	}
	ret = set_u32 (handle, "sleep_interval", interval_ms);
	nvs_close (handle);
	return ret;
}

esp_err_t
Config::get_stationary_interval (uint32_t& interval_ms) {
	nvs_handle_t handle;
	esp_err_t ret = nvs_open (NVS_NS, NVS_READONLY, &handle);
	if (ret != ESP_OK) {
		return ret;
	}
	ret = get_u32 (handle, "stationary_interval", interval_ms);
	nvs_close (handle);
	return ret;
}

esp_err_t
Config::set_stationary_interval (uint32_t interval_ms) {
	nvs_handle_t handle;
	esp_err_t ret = nvs_open (NVS_NS, NVS_READWRITE, &handle);
	if (ret != ESP_OK) {
		return ret;
	}
	ret = set_u32 (handle, "stationary_interval", interval_ms);
	nvs_close (handle);
	return ret;
}

esp_err_t
Config::get_tx_power (uint8_t& tx_power) {
	nvs_handle_t handle;
	esp_err_t ret = nvs_open (NVS_NS, NVS_READONLY, &handle);
	if (ret != ESP_OK) {
		return ret;
	}
	ret = get_u8 (handle, "tx_power", tx_power);
	nvs_close (handle);
	return ret;
}

esp_err_t
Config::set_tx_power (uint8_t tx_power) {
	nvs_handle_t handle;
	esp_err_t ret = nvs_open (NVS_NS, NVS_READWRITE, &handle);
	if (ret != ESP_OK) {
		return ret;
	}
	ret = set_u8 (handle, "tx_power", tx_power);
	nvs_close (handle);
	return ret;
}

esp_err_t
Config::get_spreading_factor (uint8_t& sf) {
	nvs_handle_t handle;
	esp_err_t ret = nvs_open (NVS_NS, NVS_READONLY, &handle);
	if (ret != ESP_OK) {
		return ret;
	}
	ret = get_u8 (handle, "spreading_factor", sf);
	nvs_close (handle);
	return ret;
}

esp_err_t
Config::set_spreading_factor (uint8_t sf) {
	nvs_handle_t handle;
	esp_err_t ret = nvs_open (NVS_NS, NVS_READWRITE, &handle);
	if (ret != ESP_OK) {
		return ret;
	}
	ret = set_u8 (handle, "spreading_factor", sf);
	nvs_close (handle);
	return ret;
}

esp_err_t
Config::get_device_name (char* name, size_t max_len) {
	nvs_handle_t handle;
	esp_err_t ret = nvs_open (NVS_NS, NVS_READONLY, &handle);
	if (ret != ESP_OK) {
		return ret;
	}
	ret = get_str (handle, "device_name", name, max_len);
	nvs_close (handle);
	return ret;
}

esp_err_t
Config::set_device_name (const char* name) {
	nvs_handle_t handle;
	esp_err_t ret = nvs_open (NVS_NS, NVS_READWRITE, &handle);
	if (ret != ESP_OK) {
		return ret;
	}
	ret = set_str (handle, "device_name", name);
	nvs_close (handle);
	return ret;
}

esp_err_t
Config::get_u32 (nvs_handle_t handle, const char* key, uint32_t& value) {
	return nvs_get_u32 (handle, key, &value);
}

esp_err_t
Config::set_u32 (nvs_handle_t handle, const char* key, uint32_t value) {
	return nvs_set_u32 (handle, key, value);
}

esp_err_t
Config::get_u8 (nvs_handle_t handle, const char* key, uint8_t& value) {
	return nvs_get_u8 (handle, key, &value);
}

esp_err_t
Config::set_u8 (nvs_handle_t handle, const char* key, uint8_t value) {
	return nvs_set_u8 (handle, key, value);
}

esp_err_t
Config::get_str (nvs_handle_t handle, const char* key, char* value, size_t max_len) {
	size_t len = max_len;
	esp_err_t ret = nvs_get_str (handle, key, value, &len);
	if (ret == ESP_ERR_NVS_NOT_FOUND) {
		strlcpy (value, DEFAULT_DEVICE_NAME, max_len);
		return ESP_OK;
	}
	return ret;
}

esp_err_t
Config::set_str (nvs_handle_t handle, const char* key, const char* value) {
	return nvs_set_str (handle, key, value);
}
