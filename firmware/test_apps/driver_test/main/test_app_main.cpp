#include "unity.h"
#include "esp_log.h"

extern "C" void app_main(void);

void setUp(void) {}
void tearDown(void) {}

extern "C" void app_main(void)
{
    ESP_LOGI("TEST", "app_main started");
    unity_run_menu();
}
