#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"
#include "lora/sx1262.hpp"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include <cstring>

TEST_CASE("LoRaDriver mode string conversion", "[lora]") {
    SECTION("mode_to_string returns correct strings") {
        REQUIRE(strcmp(LoRaDriver::mode_to_string(LoRaMode::SLEEP), "SLEEP") == 0);
        REQUIRE(strcmp(LoRaDriver::mode_to_string(LoRaMode::STANDBY), "STANDBY") == 0);
        REQUIRE(strcmp(LoRaDriver::mode_to_string(LoRaMode::TX), "TX") == 0);
        REQUIRE(strcmp(LoRaDriver::mode_to_string(LoRaMode::RX), "RX") == 0);
        REQUIRE(strcmp(LoRaDriver::mode_to_string(LoRaMode::CAD), "CAD") == 0);
    }

    SECTION("event_to_string returns correct strings") {
        REQUIRE(strcmp(LoRaDriver::event_to_string(LoRaEvent::TX_DONE), "TX_DONE") == 0);
        REQUIRE(strcmp(LoRaDriver::event_to_string(LoRaEvent::RX_DONE), "RX_DONE") == 0);
        REQUIRE(strcmp(LoRaDriver::event_to_string(LoRaEvent::RX_TIMEOUT), "RX_TIMEOUT") == 0);
        REQUIRE(strcmp(LoRaDriver::event_to_string(LoRaEvent::CAD_DONE), "CAD_DONE") == 0);
        REQUIRE(strcmp(LoRaDriver::event_to_string(LoRaEvent::CAD_DETECTED), "CAD_DETECTED") == 0);
        REQUIRE(strcmp(LoRaDriver::event_to_string(LoRaEvent::ERROR), "ERROR") == 0);
    }
}

TEST_CASE("LoRaDriver initial state", "[lora]") {
    mock_gpio::reset_gpio_mock();
    mock_spi::reset_spi_mock();

    LoRaDriver lora(SPI2_HOST, GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_6,
                    GPIO_NUM_7, GPIO_NUM_8, GPIO_NUM_9, GPIO_NUM_10);

    SECTION("Initial mode is SLEEP") {
        const char* mode_str = LoRaDriver::mode_to_string(lora.get_mode());
        REQUIRE(strcmp(mode_str, "SLEEP") == 0);
    }
}

TEST_CASE("LoRaDriver is_busy", "[lora]") {
    mock_gpio::reset_gpio_mock();
    mock_spi::reset_spi_mock();

    LoRaDriver lora(SPI2_HOST, GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_6,
                    GPIO_NUM_7, GPIO_NUM_8, GPIO_NUM_9, GPIO_NUM_10);

    SECTION("is_busy returns gpio level high") {
        mock_gpio::gpio_set_next_get_level(GPIO_NUM_9, 1);
        REQUIRE(lora.is_busy() == true);
    }

    SECTION("is_busy returns gpio level low") {
        mock_gpio::gpio_set_next_get_level(GPIO_NUM_9, 0);
        REQUIRE(lora.is_busy() == false);
    }
}
