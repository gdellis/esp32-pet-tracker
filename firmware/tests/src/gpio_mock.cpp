#include "driver/gpio.h"

namespace mock_gpio {

volatile gpio_num_t last_set_level_pin = GPIO_NUM_0;
volatile uint32_t last_set_level_value = 0;
volatile gpio_num_t last_get_level_pin = GPIO_NUM_0;
volatile uint32_t next_get_level_value = 1;

void reset_gpio_mock() {
    last_set_level_pin = GPIO_NUM_0;
    last_set_level_value = 0;
    last_get_level_pin = GPIO_NUM_0;
    next_get_level_value = 1;
}

void gpio_set_next_get_level(gpio_num_t pin, uint32_t value) {
    last_get_level_pin = pin;
    next_get_level_value = value;
}

}

extern "C" {

void gpio_reset_pin(gpio_num_t gpio_num) {
    (void)gpio_num;
}

int gpio_set_direction(gpio_num_t gpio_num, gpio_mode_t mode) {
    (void)gpio_num;
    (void)mode;
    return 0;
}

int gpio_get_level(gpio_num_t gpio_num) {
    using namespace mock_gpio;
    last_get_level_pin = gpio_num;
    return next_get_level_value;
}

__attribute__((noinline)) int gpio_set_level(gpio_num_t gpio_num, uint32_t level) {
    using namespace mock_gpio;
    last_set_level_pin = gpio_num;
    last_set_level_value = level;
    return 0;
}

int gpio_config(const gpio_config_t *conf) {
    (void)conf;
    return 0;
}

}
