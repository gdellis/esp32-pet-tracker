#include "freertos/event_groups.h"

static EventBits_t current_bits = 0;

EventGroupHandle_t xEventGroupCreateStatic(StaticEventGroup_t* buffer) {
    (void)buffer;
    return &current_bits;
}

EventBits_t xEventGroupWaitBits(
    EventGroupHandle_t xEventGroup,
    EventBits_t bitsToWaitFor,
    BaseType_t xClearOnExit,
    BaseType_t xWaitForAllBits,
    TickType_t xTicksToWait
) {
    (void)xEventGroup;
    (void)xTicksToWait;
    (void)xWaitForAllBits;

    EventBits_t result = current_bits & bitsToWaitFor;

    if (xClearOnExit) {
        current_bits &= ~bitsToWaitFor;
    }

    return result;
}

EventBits_t xEventGroupSetBits(EventGroupHandle_t xEventGroup, EventBits_t bitsToSet) {
    (void)xEventGroup;
    current_bits |= bitsToSet;
    return current_bits;
}

EventBits_t xEventGroupClearBits(EventGroupHandle_t xEventGroup, EventBits_t bitsToClear) {
    (void)xEventGroup;
    current_bits &= ~bitsToClear;
    return current_bits;
}

EventBits_t xEventGroupSetBitsFromISR(EventGroupHandle_t xEventGroup, EventBits_t bitsToSet, BaseType_t* higher_priority_task_woken) {
    (void)xEventGroup;
    (void)higher_priority_task_woken;
    current_bits |= bitsToSet;
    return current_bits;
}
