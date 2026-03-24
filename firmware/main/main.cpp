#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"

static const char* TAG = "pet-tracker";

#define LED_PIN GPIO_NUM_8
#define BUTTON_PIN GPIO_NUM_9

class GpioDriver {
public:
    GpioDriver(gpio_num_t pin, gpio_mode_t mode) : pin_(pin) {
        gpio_config_t config = {
            .pin_bit_mask = 1ULL << pin,
            .mode = mode,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };
        ESP_ERROR_CHECK(gpio_config(&config));
    }

    gpio_num_t get_pin() const { return pin_; }

protected:
    gpio_num_t pin_;
};

class LedDriver : public GpioDriver {
public:
    LedDriver(gpio_num_t pin) : GpioDriver(pin, GPIO_MODE_OUTPUT) {}

    void on() {
        ESP_ERROR_CHECK(gpio_set_level(pin_, 1));
    }

    void off() {
        ESP_ERROR_CHECK(gpio_set_level(pin_, 0));
    }

    void toggle() {
        int level = gpio_get_level(pin_);
        ESP_ERROR_CHECK(gpio_set_level(pin_, !level));
    }
};

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "ESP32 Pet Tracker starting...");

    LedDriver led(LED_PIN);
    GpioDriver button(BUTTON_PIN, GPIO_MODE_INPUT);

    led.off();

    bool led_state = false;
    int64_t last_button_time = 0;

    while (true) {
        int button_level = gpio_get_level(BUTTON_PIN);
        int64_t now = esp_timer_get_time();

        if (button_level == 0 && (now - last_button_time) > 200000) {
            led_state = !led_state;
            if (led_state) {
                led.on();
            } else {
                led.off();
            }
            last_button_time = now;
            ESP_LOGI(TAG, "Button pressed, LED toggled to %s", led_state ? "ON" : "OFF");
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
