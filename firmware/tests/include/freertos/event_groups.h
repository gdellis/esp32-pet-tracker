#ifndef TEST_MOCKS_FREERTOS_EVENT_GROUPS_H
#define TEST_MOCKS_FREERTOS_EVENT_GROUPS_H

#include <stdint.h>
#include "FreeRTOS.h"

typedef void* EventGroupHandle_t;

typedef struct {
    uint32_t bits;
} StaticEventGroup_t;

#define EVENTS_WAIT_ALL 0
#define EVENTS_WAIT_ANY 1

#define EventBits_t uint32_t

EventGroupHandle_t xEventGroupCreateStatic(StaticEventGroup_t* buffer);
EventBits_t xEventGroupWaitBits(
    EventGroupHandle_t xEventGroup,
    EventBits_t bitsToWaitFor,
    BaseType_t xClearOnExit,
    BaseType_t xWaitForAllBits,
    TickType_t xTicksToWait
);
EventBits_t xEventGroupSetBits(EventGroupHandle_t xEventGroup, EventBits_t bitsToSet);
EventBits_t xEventGroupClearBits(EventGroupHandle_t xEventGroup, EventBits_t bitsToClear);
EventBits_t xEventGroupSetBitsFromISR(EventGroupHandle_t xEventGroup, EventBits_t bitsToSet, BaseType_t* higher_priority_task_woken);

#endif
