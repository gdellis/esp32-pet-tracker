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

typedef enum {
    GPIO_PULLUP_DISABLE = 0,
    GPIO_PULLUP_ENABLE = 1
} gpio_pullup_t;

typedef enum {
    GPIO_PULLDOWN_DISABLE = 0,
    GPIO_PULLDOWN_ENABLE = 1
} gpio_pulldown_t;

typedef enum {
    GPIO_INTR_DISABLE = 0,
    GPIO_INTR_ANYEDGE = 2,
    GPIO_INTR_NEGEDGE = 3,
    GPIO_INTR_POSEDGE = 4
} gpio_int_type_t;

typedef int gpio_num_t;

#define GPIO_PULLUP_DISABLE GPIO_PULLUP_DISABLE
#define GPIO_PULLDOWN_DISABLE GPIO_PULLDOWN_DISABLE
#define GPIO_INTR_DISABLE GPIO_INTR_DISABLE

typedef struct {
    uint64_t pin_bit_mask;
    gpio_mode_t mode;
    gpio_pullup_t pull_up_en;
    gpio_pulldown_t pull_down_en;
    gpio_int_type_t intr_type;
} gpio_config_t;

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
extern volatile gpio_num_t last_set_level_pin;
extern volatile uint32_t last_set_level_value;
extern volatile gpio_num_t last_get_level_pin;
extern volatile uint32_t next_get_level_value;

void reset_gpio_mock();
void gpio_set_next_get_level(gpio_num_t pin, uint32_t value);
void trigger_dio1_isr();
}

#endif
