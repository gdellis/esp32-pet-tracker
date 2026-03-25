#ifndef TEST_MOCKS_ESP_ERR_H
#define TEST_MOCKS_ESP_ERR_H

#include <stdint.h>
#include <cstdio>
#include <cstdlib>

typedef int32_t esp_err_t;

#define ESP_OK 0
#define ESP_ERR_INVALID_ARG 0x101
#define ESP_ERR_INVALID_STATE 0x102
#define ESP_FAIL -1

#define ESP_ERROR_CHECK(x) do { \
    esp_err_t err = (x); \
    if (err != ESP_OK) { \
        fprintf(stderr, "ESP_ERROR_CHECK failed at %s:%d\n", __FILE__, __LINE__); \
        exit(1); \
    } \
} while(0)

#endif
