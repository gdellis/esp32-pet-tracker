#include "unity.h"
#include <stdint.h>
#include <stdbool.h>

#define GPIO_MODE_INPUT         0x1
#define GPIO_MODE_OUTPUT        0x2
#define GPIO_MODE_INPUT_OUTPUT  0x3

typedef enum {
    GPIO_NUM_0 = 0,
    GPIO_NUM_1 = 1,
    GPIO_NUM_2 = 2,
    GPIO_NUM_3 = 3,
    GPIO_NUM_4 = 4,
    GPIO_NUM_5 = 5,
    GPIO_NUM_6 = 6,
    GPIO_NUM_7 = 7,
    GPIO_NUM_8 = 8,
    GPIO_NUM_9 = 9,
} gpio_num_t;

TEST_CASE("GPIO mode definitions exist", "[gpio]")
{
    TEST_ASSERT(GPIO_MODE_INPUT != GPIO_MODE_OUTPUT);
    TEST_ASSERT(GPIO_MODE_INPUT_OUTPUT != 0);
    TEST_ASSERT(GPIO_MODE_INPUT_OUTPUT == (GPIO_MODE_INPUT | GPIO_MODE_OUTPUT));
}

TEST_CASE("GPIO number definitions", "[gpio]")
{
    TEST_ASSERT_EQUAL(8, GPIO_NUM_8);
    TEST_ASSERT_EQUAL(9, GPIO_NUM_9);
}

TEST_CASE("GPIO mode values are correct", "[gpio]")
{
    TEST_ASSERT_EQUAL(0x1, GPIO_MODE_INPUT);
    TEST_ASSERT_EQUAL(0x2, GPIO_MODE_OUTPUT);
    TEST_ASSERT_EQUAL(0x3, GPIO_MODE_INPUT_OUTPUT);
}
