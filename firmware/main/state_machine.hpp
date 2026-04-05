#pragma once

#include "accelerometer.hpp"
#include "ble.hpp"
#include "button_handler.hpp"
#include "config.hpp"
#include "geofence.hpp"
#include "gps.hpp"
#include "led_driver.hpp"
#include "lora/sx1262.hpp"
#include <stdint.h>

enum class TrackerState { INIT, IDLE, ACQUIRING_GPS, TRANSMITTING, DEEP_SLEEP, ERROR };

enum class WakeSource { NONE, TIMER, BUTTON, MOTION };

constexpr uint32_t GPS_TIMEOUT_MS = 60000;
constexpr uint8_t MAX_TX_RETRIES = 3;
constexpr uint32_t TX_RETRY_BASE_DELAY_MS = 1000;
constexpr uint32_t INACTIVITY_THRESHOLD_MS = 30000;
constexpr uint32_t BUTTON_WAKE_SLEEP_MS = 5000;

struct TrackerContext {
	TrackerState state;
	WakeSource last_wake;
	uint32_t wake_count;
	bool gps_fix_obtained;
	bool is_moving;
	int64_t last_activity_time;
	uint8_t tx_retry_count;
};

class TrackerStateMachine {
  public:
	TrackerStateMachine (Gps& gps, LoRaDriver& lora, Accelerometer& accel, BleServer& ble,
						 LedDriver& led);

	void init ();
	void run ();

	TrackerState
	get_state () const {
		return ctx_.state;
	}
	WakeSource
	get_last_wake () const {
		return ctx_.last_wake;
	}
	uint32_t
	get_wake_count () const {
		return ctx_.wake_count;
	}

  private:
	void transition_to (TrackerState new_state);
	WakeSource get_wake_source ();
	void sleep (uint32_t duration_ms);
	void check_motion ();
	void check_geofence ();
	void update_activity_time ();
	bool check_button ();
	esp_err_t transmit_location ();
	esp_err_t try_lora_send (const GpsData& data, bool valid_fix);
	esp_err_t try_ble_fallback (const GpsData& data, bool valid_fix);
	uint32_t get_sleep_duration () const;
	void configure_wakeup_sources ();

	TrackerContext ctx_;
	Gps& gps_;
	LoRaDriver& lora_;
	Accelerometer& accel_;
	BleServer& ble_;
	LedDriver& led_;
	ButtonHandler button_;
	TrackerConfig config_;
	bool geofence_breach_ = false;
};
