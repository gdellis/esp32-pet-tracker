#ifndef DEEP_SLEEP_HPP
#define DEEP_SLEEP_HPP

#include <stdint.h>
#include "esp_sleep.h"

class DeepSleep {
public:
    static constexpr uint64_t DEFAULT_WAKEUP_TIME_US = 60 * 1000000ULL;

    static void sleep(uint64_t duration_us = DEFAULT_WAKEUP_TIME_US)
    {
        esp_deep_sleep_start();
    }

    static void enable_timer_wakeup(uint64_t duration_us)
    {
        esp_sleep_enable_timer_wakeup(duration_us);
    }

    static void enable_gpio_wakeup(gpio_num_t gpio_num, esp_sleep_ext1_wakeup_mode_t mode)
    {
        uint64_t mask = 1ULL << gpio_num;
        esp_sleep_enable_ext1_wakeup(mask, mode);
    }

    static bool is_wakeup_caused_by_timer(void)
    {
        return esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER;
    }

    static bool is_wakeup_caused_by_gpio(void)
    {
        return esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT1;
    }

    static uint64_t get_wakeup_cause(void)
    {
        return esp_sleep_get_wakeup_cause();
    }
};

#endif
