#ifndef TEST_MOCKS_FREERTOS_FREERTOS_H
#define TEST_MOCKS_FREERTOS_FREERTOS_H

#include <stdint.h>

#define pdTRUE 1
#define pdFALSE 0
#define pdMS_TO_TICK_COUNT(ms) (ms)
#define portMAX_DELAY 0xffffffffUL
#define portYIELD_FROM_ISR()

typedef int32_t BaseType_t;
typedef uint32_t UBaseType_t;
typedef void* TaskHandle_t;
typedef uint32_t TickType_t;

#endif
