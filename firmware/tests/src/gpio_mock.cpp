#include "driver/gpio.h"
#include "driver/gpio_isr.h"

namespace mock_gpio {

volatile gpio_num_t last_set_level_pin = GPIO_NUM_0;
volatile uint32_t last_set_level_value = 0;
volatile gpio_num_t last_get_level_pin = GPIO_NUM_0;
volatile uint32_t next_get_level_value = 1;

void (*registered_isr_handler)(void*) = nullptr;
void* registered_isr_arg = nullptr;

void reset_gpio_mock() {
    last_set_level_pin = GPIO_NUM_0;
    last_set_level_value = 0;
    last_get_level_pin = GPIO_NUM_0;
    next_get_level_value = 1;
    registered_isr_handler = nullptr;
    registered_isr_arg = nullptr;
}

void gpio_set_next_get_level(gpio_num_t pin, uint32_t value) {
    last_get_level_pin = pin;
    next_get_level_value = value;
}

void trigger_dio1_isr() {
    if (registered_isr_handler) {
        registered_isr_handler(registered_isr_arg);
    }
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

int gpio_set_intr_type(gpio_num_t gpio_num, gpio_int_type_t intr_type) {
    (void)gpio_num;
    (void)intr_type;
    return 0;
}

int gpio_isr_handler_add(gpio_num_t gpio_num, gpio_isr_t isr_handler, void* args) {
    (void)gpio_num;
    using namespace mock_gpio;
    registered_isr_handler = isr_handler;
    registered_isr_arg = args;
    return 0;
}

}
