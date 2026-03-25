#ifndef TEST_MOCKS_DRIVER_GPIO_H
#define TEST_MOCKS_DRIVER_GPIO_H

#include <stdint.h>

typedef enum {
    GPIO_MODE_INPUT = 0,
    GPIO_MODE_OUTPUT,
    GPIO_MODE_INPUT_OUTPUT,
    GPIO_MODE_INPUT_OUTPUT_OD,
    GPIO_MODE_DISABLE
} gpio_mode_t;

typedef int gpio_num_t;
typedef uint32_t gpio_config_t;

#define GPIO_NUM_0  ((gpio_num_t)0)
#define GPIO_NUM_1  ((gpio_num_t)1)
#define GPIO_NUM_2  ((gpio_num_t)2)
#define GPIO_NUM_3  ((gpio_num_t)3)
#define GPIO_NUM_4  ((gpio_num_t)4)
#define GPIO_NUM_5  ((gpio_num_t)5)
#define GPIO_NUM_6  ((gpio_num_t)6)
#define GPIO_NUM_7  ((gpio_num_t)7)
#define GPIO_NUM_8  ((gpio_num_t)8)
#define GPIO_NUM_9  ((gpio_num_t)9)
#define GPIO_NUM_10 ((gpio_num_t)10)
#define GPIO_NUM_15 ((gpio_num_t)15)

#ifdef __cplusplus
extern "C" {
#endif

void gpio_reset_pin(gpio_num_t gpio_num);
int gpio_set_direction(gpio_num_t gpio_num, gpio_mode_t mode);
int gpio_get_level(gpio_num_t gpio_num);
int gpio_set_level(gpio_num_t gpio_num, uint32_t level);
int gpio_config(const gpio_config_t *conf);

#ifdef __cplusplus
}
#endif

namespace mock_gpio {
extern gpio_num_t last_set_level_pin;
extern uint32_t last_set_level_value;
extern gpio_num_t last_get_level_pin;
extern uint32_t next_get_level_value;

void reset_gpio_mock();
void gpio_set_next_get_level(gpio_num_t pin, uint32_t value);
}

#endif
