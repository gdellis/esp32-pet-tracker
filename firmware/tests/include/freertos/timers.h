#ifndef TEST_MOCKS_FREERTOS_TIMERS_H
#define TEST_MOCKS_FREERTOS_TIMERS_H

#include "FreeRTOS.h"

typedef void* TimerHandle_t;
typedef void (*TimerCallbackFunction_t)(TimerHandle_t xTimer);
typedef void (*PendedFunction_t)(uint32_t, uint32_t);

#define pdMS_TO_TICKS(ms) (ms)

BaseType_t xTimerPendFunctionCallFromISR(
    PendedFunction_t xFunctionToPend,
    void *pvParameter1,
    uint32_t ulParameter2,
    BaseType_t *pxHigherPriorityTaskWoken
);

#endif
