#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"
#include "led_driver.hpp"
#include "driver/gpio.h"
#include <cstring>

TEST_CASE("LedDriver state transitions", "[led]") {
    mock_gpio::reset_gpio_mock();
    mock_esp_timer::reset_timer_mock();

    SECTION("on() sets GPIO high") {
        LedDriver led(GPIO_NUM_8);
        led.on();

        REQUIRE(mock_gpio::last_set_level_pin == GPIO_NUM_8);
        REQUIRE(mock_gpio::last_set_level_value == 1);
    }

    SECTION("off() sets GPIO low") {
        LedDriver led(GPIO_NUM_8);
        led.off();

        REQUIRE(mock_gpio::last_set_level_pin == GPIO_NUM_8);
        REQUIRE(mock_gpio::last_set_level_value == 0);
    }

    SECTION("toggle() flips state") {
        LedDriver led(GPIO_NUM_8);

        mock_gpio::gpio_set_next_get_level(GPIO_NUM_8, 0);
        led.toggle();
        REQUIRE(mock_gpio::last_set_level_value == 1);

        mock_gpio::gpio_set_next_get_level(GPIO_NUM_8, 1);
        led.toggle();
        REQUIRE(mock_gpio::last_set_level_value == 0);
    }

    SECTION("Constructor defaults to off") {
        LedDriver led(GPIO_NUM_8);
        REQUIRE(led.get_state() == false);
    }

    SECTION("on() updates internal state") {
        LedDriver led(GPIO_NUM_8);
        led.on();
        REQUIRE(led.get_state() == true);
    }

    SECTION("off() updates internal state") {
        LedDriver led(GPIO_NUM_8);
        led.on();
        led.off();
        REQUIRE(led.get_state() == false);
    }
}
