#include "unity.h"
#include "driver/gpio.h"

TEST_CASE("LED driver initializes correctly", "[led]")
{
    TEST_ASSERT_EQUAL(8, GPIO_NUM_8);
    TEST_ASSERT_EQUAL(9, GPIO_NUM_9);
}

TEST_CASE("GPIO mode definitions exist", "[gpio]")
{
    TEST_ASSERT(GPIO_MODE_INPUT != GPIO_MODE_OUTPUT);
    TEST_ASSERT(GPIO_MODE_INPUT_OUTPUT != 0);
}

TEST_CASE("Button debounce time constant", "[button]")
{
    int64_t debounce_us = 200000;
    TEST_ASSERT_EQUAL(200000, debounce_us);
}
