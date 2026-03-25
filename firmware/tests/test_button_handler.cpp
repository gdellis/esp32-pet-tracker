#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"
#include "button_handler.hpp"
#include "esp_timer.h"
#include <cstring>

TEST_CASE("ButtonHandler debounce", "[button]") {
    mock_esp_timer::reset_timer_mock();
    mock_gpio::reset_gpio_mock();

    ButtonHandler btn(GPIO_NUM_9, 200000);

    SECTION("First press returns true") {
        mock_gpio::gpio_set_next_get_level(GPIO_NUM_9, 0);
        mock_esp_timer::timer_advance(0);

        REQUIRE(btn.check_pressed() == true);
    }

    SECTION("Rapid second press returns false (debounce)") {
        mock_gpio::gpio_set_next_get_level(GPIO_NUM_9, 0);
        btn.check_pressed();

        mock_gpio::gpio_set_next_get_level(GPIO_NUM_9, 0);
        mock_esp_timer::timer_advance(100000);

        REQUIRE(btn.check_pressed() == false);
    }

    SECTION("Press after debounce time returns true") {
        mock_gpio::gpio_set_next_get_level(GPIO_NUM_9, 0);
        btn.check_pressed();

        mock_gpio::gpio_set_next_get_level(GPIO_NUM_9, 0);
        mock_esp_timer::timer_advance(250000);

        REQUIRE(btn.check_pressed() == true);
    }

    SECTION("High level returns false") {
        mock_gpio::gpio_set_next_get_level(GPIO_NUM_9, 1);
        mock_esp_timer::timer_advance(0);

        REQUIRE(btn.check_pressed() == false);
    }
}
