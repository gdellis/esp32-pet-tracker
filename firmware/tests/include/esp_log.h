#ifndef TEST_MOCKS_ESP_LOG_H
#define TEST_MOCKS_ESP_LOG_H

#define ESP_LOGD(tag, format, ...) do {} while(0)
#define ESP_LOGI(tag, format, ...) do {} while(0)
#define ESP_LOGW(tag, format, ...) do {} while(0)
#define ESP_LOGE(tag, format, ...) do {} while(0)

#define ESP_ERROR_CHECK(x) do { (void)(x); } while(0)

#define ESP_OK 0
#define ESP_ERR_TIMEOUT 0x101
#define ESP_ERR_INVALID_ARG 0x102
#define ESP_ERR_INVALID_SIZE 0x103
#define ESP_ERR_INVALID_STATE 0x104

#endif
