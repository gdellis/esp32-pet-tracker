#include "esp_timer.h"
#include <cstring>

namespace mock_esp_timer {

int64_t current_time = 0;
int64_t time_increment = 1000;

void reset_timer_mock() {
    current_time = 0;
    time_increment = 1000;
}

void timer_advance(int64_t us) {
    current_time += us;
}

}

extern "C" {

int64_t esp_timer_get_time() {
    return mock_esp_timer::current_time;
}

void esp_timer__set_time(int64_t time) {
    mock_esp_timer::current_time = time;
}

int64_t esp_timer__get_increment() {
    return mock_esp_timer::time_increment;
}

}
