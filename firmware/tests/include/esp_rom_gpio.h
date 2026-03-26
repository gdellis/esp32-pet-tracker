#ifndef TEST_MOCKS_ESP_ROM_GPIO_H
#define TEST_MOCKS_ESP_ROM_GPIO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void esp_rom_gpio_connect_spi_signal(int gpio, int signal, bool invert, uint8_t mode);
void esp_rom_gpio_pad_select_gpio(uint8_t gpio);
void esp_rom_delay_us(uint32_t us);

#ifdef __cplusplus
}
#endif

#endif
