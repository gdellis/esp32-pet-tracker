#pragma once

#include "esp_bt.h"
#include "esp_err.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <stdint.h>
#include <string.h>

constexpr uint16_t BLE_SERVICE_UUID = 0x1810;
constexpr uint16_t BLE_CHAR_LOCATION_UUID = 0x2A67;
constexpr uint16_t BLE_CHAR_NAME_UUID = 0x2A99;
constexpr uint16_t BLE_CHAR_ALERT_UUID = 0x2A06;

constexpr uint16_t BLE_DEVICE_NAME_MAX = 32;

constexpr uint16_t BLE_MIN_INTERVAL_MS = 20;
constexpr uint16_t BLE_MAX_INTERVAL_MS = 40;
constexpr uint16_t BLE_MUTEX_TIMEOUT_MS = 100;

struct BleLocationData {
	int32_t latitude;
	int32_t longitude;
	int32_t altitude;
	uint32_t timestamp;
	uint8_t valid;
};

struct BleAlertData {
	uint8_t alert_type;
	uint8_t zone_index;
	int32_t latitude;
	int32_t longitude;
	int32_t altitude;
	uint32_t timestamp;
};

class BleServer {
  public:
	BleServer ();
	~BleServer ();

	esp_err_t init ();
	esp_err_t start ();
	esp_err_t stop ();
	esp_err_t update_location (const BleLocationData& location);
	esp_err_t send_alert (const BleAlertData& alert);
	esp_err_t set_device_name (const char* name);

	bool is_connected () const;

  private:
	static void gatts_event_handler (esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if,
									 esp_ble_gatts_cb_param_t* param);
	static void gap_event_handler (esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t* param);

	void on_gatts_event (esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if,
						 esp_ble_gatts_cb_param_t* param);
	void on_gap_event (esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t* param);

	void register_app ();
	void create_services ();
	void add_characteristics ();
	void start_advertising ();
	void send_location_response (uint16_t conn_id, uint16_t trans_id, uint16_t handle);
	void send_name_response (uint16_t conn_id, uint16_t trans_id, uint16_t handle);

	SemaphoreHandle_t mutex_;

	uint16_t service_handle_ = 0;
	uint16_t location_char_handle_ = 0;
	uint16_t name_char_handle_ = 0;
	uint16_t alert_char_handle_ = 0;
	uint16_t connection_id_ = 0;
	bool connected_ = false;
	bool started_ = false;
	bool initialized_ = false;

	char device_name_[BLE_DEVICE_NAME_MAX] = "PetTracker";
	BleLocationData current_location_ = {};
	BleAlertData current_alert_ = {};

	static BleServer* instance_;
};
