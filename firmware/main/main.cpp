#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_sleep.h"

#include "board_config.h"
#include "gpio_driver.hpp"
#include "led_driver.hpp"
#include "button_handler.hpp"
#include "deep_sleep.hpp"
#include "gps.hpp"

static const char* TAG = "pet-tracker";

#define SLEEP_TIMEOUT_MS 30000
#define DEEP_SLEEP_DURATION_US (5 * 60 * 1000000ULL)

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "ESP32 Pet Tracker starting...");

    DeepSleep::enable_timer_wakeup(DEEP_SLEEP_DURATION_US);
    DeepSleep::enable_gpio_wakeup(BOARD_BUTTON_PIN, ESP_EXT1_WAKEUP_ANY_LOW);

    switch (DeepSleep::get_wakeup_cause()) {
    case ESP_SLEEP_WAKEUP_TIMER:
        ESP_LOGI(TAG, "Woke up from deep sleep (timer)");
        break;
    case ESP_SLEEP_WAKEUP_EXT1:
        ESP_LOGI(TAG, "Woke up from deep sleep (button)");
        break;
    default:
        ESP_LOGI(TAG, "Power-on reset");
        break;
    }

    LedDriver led(BOARD_LED_PIN);
    ButtonHandler button(BOARD_BUTTON_PIN, 200000);
    Gps gps(GPS_UART_NUM);

    if (gps.init()) {
        ESP_LOGI(TAG, "GPS initialized");
    }

    led.off();

    bool led_state = false;
    int64_t last_activity_time = esp_timer_get_time() / 1000;

    while (true) {
        if (button.check_pressed()) {
            last_activity_time = esp_timer_get_time() / 1000;
            led_state = !led_state;
            if (led_state) {
                led.on();
            } else {
                led.off();
            }
            ESP_LOGI(TAG, "Button pressed, LED toggled to %s", led_state ? "ON" : "OFF");
        }

        if (gps.update() && gps.has_fix()) {
            const auto& data = gps.get_data();
            ESP_LOGI(TAG, "GPS: lat=%.6f, lon=%.6f, alt=%.1f, satellites=%d",
                     data.latitude, data.longitude, data.altitude, data.satellites);
        }

        int64_t current_time = esp_timer_get_time() / 1000;
        if (current_time - last_activity_time > SLEEP_TIMEOUT_MS) {
            ESP_LOGI(TAG, "Entering deep sleep for %llu seconds...", DEEP_SLEEP_DURATION_US / 1000000);
            led.off();
            DeepSleep::sleep();
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
