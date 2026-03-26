#ifndef TEST_MOCKS_DRIVER_GPIO_ISR_H
#define TEST_MOCKS_DRIVER_GPIO_ISR_H

#include <stdint.h>
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*gpio_isr_t)(void*);
typedef void* gpio_isr_handler_t;

int gpio_set_intr_type(gpio_num_t gpio_num, gpio_int_type_t intr_type);
int gpio_isr_handler_add(gpio_num_t gpio_num, gpio_isr_t isr_handler, void* args);

#ifdef __cplusplus
}
#endif

#endif
