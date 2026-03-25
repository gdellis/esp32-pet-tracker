#ifndef TEST_MOCKS_ESP_TIMER_H
#define TEST_MOCKS_ESP_TIMER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int64_t esp_timer_get_time(void);
void esp_timer__set_time(int64_t time);

#ifdef __cplusplus
}
#endif

namespace mock_esp_timer {
extern int64_t current_time;
extern int64_t time_increment;

void reset_timer_mock();
void timer_advance(int64_t us);
}

#endif
