#include "state_machine.hpp"
#include "esp_log.h"
#include "esp_sleep.h"
#include "driver/gpio.h"

static const char* TAG = "tracker";

TrackerStateMachine::TrackerStateMachine()
    : state_(TrackerState::INIT),
      wake_count_(0),
      last_activity_time_(0),
      gps_fix_obtained_(false),
      is_moving_(false) {}

void TrackerStateMachine::init() {
    transition_to(TrackerState::IDLE);
}

void TrackerStateMachine::run() {
    while (true) {
        switch (state_) {
        case TrackerState::INIT:
            transition_to(TrackerState::IDLE);
            break;

        case TrackerState::IDLE:
            last_activity_time_ = esp_timer_get_time() / 1000;
            transition_to(TrackerState::ACQUIRING_GPS);
            break;

        case TrackerState::ACQUIRING_GPS: {
            int64_t start_time = esp_timer_get_time() / 1000;
            bool timeout = false;

            while (!gps_fix_obtained_ && !timeout) {
                if (gps.update()) {
                    if (gps.has_fix()) {
                        gps_fix_obtained_ = true;
                        ESP_LOGI(TAG, "GPS fix obtained");
                        break;
                    }
                }

                int64_t elapsed = (esp_timer_get_time() / 1000) - start_time;
                if (elapsed > GPS_TIMEOUT_MS) {
                    timeout = true;
                    ESP_LOGW(TAG, "GPS timeout after %ums", GPS_TIMEOUT_MS);
                }

                vTaskDelay(pdMS_TO_TICKS(100));
            }

            transition_to(TrackerState::TRANSMITTING);
            break;
        }

        case TrackerState::TRANSMITTING:
            if (gps.has_fix()) {
                const auto& data = gps.get_data();
                ESP_LOGI(TAG, "TX: lat=%.6f, lon=%.6f, alt=%.1f",
                         data.latitude, data.longityde, data.altitude);
            } else {
                ESP_LOGI(TAG, "TX: no GPS fix, will send invalid");
            }

            determine_sleep_duration(is_moving_);
            transition_to(TrackerState::DEEP_SLEEP);
            break;

        case TrackerState::DEEP_SLEEP: {
            uint32_t sleep_ms = is_moving_ ? DEFAULT_SLEEP_INTERVAL_MS : STATIONARY_SLEEP_INTERVAL_MS;
            ESP_LOGI(TAG, "Entering deep sleep for %u ms (moving=%s)",
                     sleep_ms, is_moving_ ? "yes" : "no");
            sleep(sleep_ms);

            WakeSource wake = get_wake_source();
            ESP_LOGI(TAG, "Woke up from: %s",
                     wake == WakeSource::TIMER ? "timer" :
                     wake == WakeSource::BUTTON ? "button" :
                     wake == WakeSource::MOTION ? "motion" : "unknown");

            if (wake == WakeSource::MOTION) {
                is_moving_ = true;
            } else if (wake == WakeSource::TIMER) {
                is_moving_ = false;
            }

            wake_count_++;
            gps_fix_obtained_ = false;
            transition_to(TrackerState::IDLE);
            break;
        }

        case TrackerState::ERROR:
            ESP_LOGE(TAG, "Error state, resetting...");
            transition_to(TrackerState::IDLE);
            break;
        }
    }
}

void TrackerStateMachine::transition_to(TrackerState new_state) {
    state_ = new_state;
    ESP_LOGD(TAG, "State -> %s",
             state_ == TrackerState::INIT ? "INIT" :
             state_ == TrackerState::IDLE ? "IDLE" :
             state_ == TrackerState::ACQUIRING_GPS ? "ACQUIRING_GPS" :
             state_ == TrackerState::TRANSMITTING ? "TRANSMITTING" :
             state_ == TrackerState::DEEP_SLEEP ? "DEEP_SLEEP" : "ERROR");
}

WakeSource TrackerStateMachine::get_wake_source() {
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();

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
}

void TrackerStateMachine::sleep(uint32_t duration_ms) {
    DeepSleep::enable_timer_wakeup(duration_ms * 1000ULL);
    DeepSleep::sleep();
}

void TrackerStateMachine::determine_sleep_duration(bool is_moving) {
    is_moving_ = is_moving;
}
