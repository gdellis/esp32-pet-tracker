#include "state_machine.hpp"
#include "deep_sleep.hpp"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "freertos/task.h"

static const char* TAG = "tracker";

TrackerStateMachine::TrackerStateMachine (Gps& gps, LoRaDriver& lora, Accelerometer& accel)
	: ctx_ ({ TrackerState::INIT, WakeSource::NONE, 0, false, false, 0 }), gps_ (gps), lora_ (lora),
	  accel_ (accel) {}

void
TrackerStateMachine::init () {
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

			transition_to (TrackerState::TRANSMITTING);
			break;
		}

		case TrackerState::TRANSMITTING:
			if (gps_.has_fix ()) {
				const auto& data = gps_.get_data ();
				ESP_LOGI (TAG, "TX: lat=%.6f, lon=%.6f, alt=%.1f", data.latitude, data.longitude,
						  data.altitude);
			} else {
				ESP_LOGI (TAG, "TX: no GPS fix, will send invalid");
			}

			check_motion ();
			transition_to (TrackerState::DEEP_SLEEP);
			break;

		case TrackerState::DEEP_SLEEP: {
			uint32_t sleep_ms
				= ctx_.is_moving ? DEFAULT_SLEEP_INTERVAL_MS : STATIONARY_SLEEP_INTERVAL_MS;
			ESP_LOGI (TAG, "Entering deep sleep for %u ms (moving=%s)", sleep_ms,
					  ctx_.is_moving ? "yes" : "no");
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
			update_activity_time ();
			transition_to (TrackerState::IDLE);
			break;
		}

		case TrackerState::ERROR:
			ESP_LOGE (TAG, "Error state, resetting...");
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
		// If no motion detected for a while, consider stationary
		int64_t now = esp_timer_get_time () / 1000;
		if (now - ctx_.last_activity_time > 30000) { // 30 seconds
			ctx_.is_moving = false;
		}
	}
}

void
TrackerStateMachine::update_activity_time () {
	ctx_.last_activity_time = esp_timer_get_time () / 1000;
}
