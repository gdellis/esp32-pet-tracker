#ifndef DEEP_SLEEP_HPP
#define DEEP_SLEEP_HPP

#include <stdint.h>
#include "esp_sleep.h"

/**
 * @brief Deep sleep utility for power management
 */
class DeepSleep {
public:
    static constexpr uint64_t DEFAULT_WAKEUP_TIME_US = 60 * 1000000ULL;

    /**
     * @brief Enter deep sleep
     * @param duration_us Sleep duration in microseconds (default 60s)
     */
    static void sleep(uint64_t duration_us = DEFAULT_WAKEUP_TIME_US)
    {
        esp_deep_sleep_start();
    }

    /**
     * @brief Enable timer wakeup from deep sleep
     * @param duration_us Wakeup time in microseconds
     */
    static void enable_timer_wakeup(uint64_t duration_us)
    {
        esp_sleep_enable_timer_wakeup(duration_us);
    }

    /**
     * @brief Enable GPIO wakeup from deep sleep
     * @param gpio_num GPIO pin number
     * @param mode Wakeup mode (e.g., ESP_EXT1_WAKEUP_ANY_LOW)
     */
    static void enable_gpio_wakeup(gpio_num_t gpio_num, esp_sleep_ext1_wakeup_mode_t mode)
    {
        uint64_t mask = 1ULL << gpio_num;
        esp_sleep_enable_ext1_wakeup(mask, mode);
    }

    /**
     * @brief Check if wakeup was caused by timer
     * @return true if timer triggered wakeup
     */
    static bool is_wakeup_caused_by_timer(void)
    {
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
        return (esp_sleep_get_wakeup_causes() & ESP_SLEEP_WAKEUP_TIMER) != 0;
#else
        return esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER;
#endif
    }

    /**
     * @brief Check if wakeup was caused by GPIO
     * @return true if GPIO triggered wakeup
     */
    static bool is_wakeup_caused_by_gpio(void)
    {
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
        return (esp_sleep_get_wakeup_causes() & ESP_SLEEP_WAKEUP_EXT1) != 0;
#else
        return esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT1;
#endif
    }

    /**
     * @brief Get the wakeup cause
     * @return Wakeup cause code
     */
    static uint64_t get_wakeup_cause(void)
    {
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
        return esp_sleep_get_wakeup_causes();
#else
        return esp_sleep_get_wakeup_cause();
#endif
    }
};

#endif
