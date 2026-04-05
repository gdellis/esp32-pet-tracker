#include "state_machine.hpp"
#include "deep_sleep.hpp"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "freertos/task.h"
#include <string.h>

static const char* TAG = "tracker";

TrackerStateMachine::TrackerStateMachine (Gps& gps, LoRaDriver& lora, Accelerometer& accel,
										  BleServer& ble)
	: ctx_ ({ TrackerState::INIT, WakeSource::NONE, 0, false, false, 0, 0 }), gps_ (gps),
	  lora_ (lora), accel_ (accel), ble_ (ble) {}

void
TrackerStateMachine::init () {
	Config::init ();
	Config::load (config_);

	lora_.set_tx_power (config_.tx_power);
	lora_.set_spreading_factor (config_.spreading_factor);

	transition_to (TrackerState::IDLE);
}

void
TrackerStateMachine::run () {
	while (true) {
		switch (ctx_.state) {
		case TrackerState::INIT:
			transition_to (TrackerState::IDLE);
			break;

		case TrackerState::IDLE:
			update_activity_time ();
			transition_to (TrackerState::ACQUIRING_GPS);
			break;

		case TrackerState::ACQUIRING_GPS: {
			gps_.power_on ();
			int64_t start_time = esp_timer_get_time () / 1000;
			bool timeout = false;

			while (!ctx_.gps_fix_obtained && !timeout) {
				if (gps_.update ()) {
					if (gps_.has_fix ()) {
						ctx_.gps_fix_obtained = true;
						ESP_LOGI (TAG, "GPS fix obtained");
						break;
					}
				}

				int64_t elapsed = (esp_timer_get_time () / 1000) - start_time;
				if (elapsed > GPS_TIMEOUT_MS) {
					timeout = true;
					ESP_LOGW (TAG, "GPS timeout after %ums", GPS_TIMEOUT_MS);
				}

				vTaskDelay (pdMS_TO_TICKS (100));
			}

			gps_.power_off ();
			transition_to (TrackerState::TRANSMITTING);
			break;
		}

		case TrackerState::TRANSMITTING: {
			const auto& data = gps_.get_data ();
			if (ctx_.gps_fix_obtained) {
				ESP_LOGI (TAG, "TX: lat=%.6f, lon=%.6f, alt=%.1f", data.latitude, data.longitude,
						  data.altitude);
			} else {
				ESP_LOGI (TAG, "TX: no GPS fix, will send invalid");
			}

			esp_err_t err = transmit_location ();
			if (err != ESP_OK) {
				ESP_LOGE (TAG, "TX failed after retries, entering error state");
				transition_to (TrackerState::ERROR);
			} else {
				transition_to (TrackerState::DEEP_SLEEP);
			}
			break;
		}

		case TrackerState::DEEP_SLEEP: {
			uint32_t sleep_ms = get_sleep_duration ();
			ESP_LOGI (TAG, "Entering deep sleep for %u ms (moving=%s)", sleep_ms,
					  ctx_.is_moving ? "yes" : "no");

			check_motion ();

			lora_.sleep ();
			sleep (sleep_ms);

			WakeSource wake = get_wake_source ();
			ESP_LOGI (TAG, "Woke up from: %s",
					  wake == WakeSource::TIMER	   ? "timer"
					  : wake == WakeSource::BUTTON ? "button"
					  : wake == WakeSource::MOTION ? "motion"
												   : "unknown");

			if (wake == WakeSource::MOTION) {
				ctx_.is_moving = true;
			} else if (wake == WakeSource::TIMER) {
				ctx_.is_moving = false;
			}

			ctx_.wake_count++;
			ctx_.gps_fix_obtained = false;
			ctx_.tx_retry_count = 0;
			update_activity_time ();

			lora_.wake ();
			transition_to (TrackerState::IDLE);
			break;
		}

		case TrackerState::ERROR:
			ESP_LOGE (TAG, "Error state, resetting...");
			lora_.sleep ();
			sleep (config_.stationary_interval_ms);
			lora_.wake ();
			ctx_.tx_retry_count = 0;
			transition_to (TrackerState::IDLE);
			break;
		}
	}
}

void
TrackerStateMachine::transition_to (TrackerState new_state) {
	ctx_.state = new_state;
	ESP_LOGD (TAG, "State -> %s",
			  ctx_.state == TrackerState::INIT			  ? "INIT"
			  : ctx_.state == TrackerState::IDLE		  ? "IDLE"
			  : ctx_.state == TrackerState::ACQUIRING_GPS ? "ACQUIRING_GPS"
			  : ctx_.state == TrackerState::TRANSMITTING  ? "TRANSMITTING"
			  : ctx_.state == TrackerState::DEEP_SLEEP	  ? "DEEP_SLEEP"
														  : "ERROR");
}

WakeSource
TrackerStateMachine::get_wake_source () {
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
	uint64_t causes = esp_sleep_get_wakeup_causes ();
	if (causes & ESP_SLEEP_WAKEUP_TIMER) {
		return WakeSource::TIMER;
	} else if (causes & (ESP_SLEEP_WAKEUP_EXT1 | ESP_SLEEP_WAKEUP_GPIO)) {
		return WakeSource::BUTTON;
	}
#else
	esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause ();
	switch (cause) {
	case ESP_SLEEP_WAKEUP_TIMER:
		return WakeSource::TIMER;
	case ESP_SLEEP_WAKEUP_EXT1:
		return WakeSource::BUTTON;
	case ESP_SLEEP_WAKEUP_GPIO:
		return WakeSource::MOTION;
	default:
		return WakeSource::NONE;
	}
#endif
	return WakeSource::NONE;
}

void
TrackerStateMachine::sleep (uint32_t duration_ms) {
	DeepSleep::enable_timer_wakeup (duration_ms * 1000ULL);
	DeepSleep::sleep ();
}

void
TrackerStateMachine::check_motion () {
	if (accel_.has_motion ()) {
		ctx_.is_moving = true;
		accel_.clear_interrupt ();
		ESP_LOGD (TAG, "Motion detected");
	} else {
		int64_t now = esp_timer_get_time () / 1000;
		if (now - ctx_.last_activity_time > 30000) {
			ctx_.is_moving = false;
		}
	}
}

void
TrackerStateMachine::update_activity_time () {
	ctx_.last_activity_time = esp_timer_get_time () / 1000;
}

uint32_t
TrackerStateMachine::get_sleep_duration () const {
	return ctx_.is_moving ? config_.sleep_interval_ms : config_.stationary_interval_ms;
}

esp_err_t
TrackerStateMachine::transmit_location () {
	const auto& data = gps_.get_data ();
	bool valid_fix = ctx_.gps_fix_obtained;

	ctx_.tx_retry_count = 0;
	esp_err_t err;

	while (ctx_.tx_retry_count < MAX_TX_RETRIES) {
		err = try_lora_send (data, valid_fix);
		if (err == ESP_OK) {
			return ESP_OK;
		}

		ctx_.tx_retry_count++;
		if (ctx_.tx_retry_count < MAX_TX_RETRIES) {
			uint32_t delay_ms = TX_RETRY_BASE_DELAY_MS * (1 << (ctx_.tx_retry_count - 1));
			ESP_LOGW (TAG, "TX failed, retry %u/%u in %u ms", ctx_.tx_retry_count,
					  MAX_TX_RETRIES - 1, delay_ms);
			vTaskDelay (pdMS_TO_TICKS (delay_ms));
		}
	}

	ESP_LOGW (TAG, "LoRa TX failed after %u retries, trying BLE fallback", MAX_TX_RETRIES);
	err = try_ble_fallback (data, valid_fix);
	if (err == ESP_OK) {
		ESP_LOGI (TAG, "BLE fallback TX succeeded");
		return ESP_OK;
	}

	return ESP_ERR_TIMEOUT;
}

esp_err_t
TrackerStateMachine::try_lora_send (const GpsData& data, bool valid_fix) {
	uint8_t packet[23];
	memset (packet, 0, sizeof (packet));

	uint32_t device_id = config_.device_id;
	int32_t lat = valid_fix ? (int32_t)(data.latitude * 1000000) : 0;
	int32_t lon = valid_fix ? (int32_t)(data.longitude * 1000000) : 0;
	int32_t alt = valid_fix ? (int32_t)(data.altitude * 100) : 0;
	uint16_t battery = 0;
	uint8_t flags = valid_fix ? 0x01 : 0x00;
	uint32_t timestamp = (uint32_t)(esp_timer_get_time () / 1000000);

	memcpy (&packet[0], &device_id, 4);
	memcpy (&packet[4], &lat, 4);
	memcpy (&packet[8], &lon, 4);
	memcpy (&packet[12], &alt, 4);
	memcpy (&packet[16], &battery, 2);
	packet[18] = flags;
	memcpy (&packet[19], &timestamp, 4);

	return lora_.send_blocking (packet, sizeof (packet), 10000);
}

esp_err_t
TrackerStateMachine::try_ble_fallback (const GpsData& data, bool valid_fix) {
	if (!ble_.is_connected ()) {
		ESP_LOGW (TAG, "BLE not connected, cannot send fallback");
		return ESP_FAIL;
	}

	BleLocationData ble_data = {};
	ble_data.latitude = valid_fix ? (int32_t)(data.latitude * 1000000) : 0;
	ble_data.longitude = valid_fix ? (int32_t)(data.longitude * 1000000) : 0;
	ble_data.altitude = valid_fix ? (int32_t)(data.altitude * 100) : 0;
	ble_data.timestamp = (uint32_t)(esp_timer_get_time () / 1000000);
	ble_data.valid = valid_fix ? 1 : 0;

	return ble_.update_location (ble_data);
}