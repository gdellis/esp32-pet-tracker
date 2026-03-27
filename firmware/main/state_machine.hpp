#pragma once

#include <stdint.h>
#include "gps.hpp"
#include "lora/sx1262.hpp"
#include "accelerometer.hpp"

enum class TrackerState {
    INIT,
    IDLE,
    ACQUIRING_GPS,
    TRANSMITTING,
    DEEP_SLEEP,
    ERROR
};

enum class WakeSource {
    NONE,
    TIMER,
    BUTTON,
    MOTION
};

constexpr uint32_t DEFAULT_SLEEP_INTERVAL_MS = 300000;
constexpr uint32_t STATIONARY_SLEEP_INTERVAL_MS = 600000;
constexpr uint32_t GPS_TIMEOUT_MS = 60000;

struct TrackerContext {
    TrackerState state;
    WakeSource last_wake;
    uint32_t wake_count;
    bool gps_fix_obtained;
    bool is_moving;
    int64_t last_activity_time;
};

class TrackerStateMachine {
public:
    TrackerStateMachine(Gps& gps, LoRaDriver& lora, Accelerometer& accel);
    
    void init();
    void run();
    
    TrackerState get_state() const { return ctx_.state; }
    WakeSource get_last_wake() const { return ctx_.last_wake; }
    uint32_t get_wake_count() const { return ctx_.wake_count; }
    
private:
    void transition_to(TrackerState new_state);
    void determine_sleep_duration();
    
    TrackerContext ctx_;
    Gps& gps_;
    LoRaDriver& lora_;
    Accelerometer& accel_;
};
