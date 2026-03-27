#include "ble.hpp"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_defs.h"
#include "esp_err.h"

static const char* TAG = "ble";

BleServer* BleServer::instance_ = nullptr;

BleServer::BleServer() : mutex_(xSemaphoreCreateMutex()) {}

BleServer::~BleServer() {
    if (mutex_ != nullptr) {
        vSemaphoreDelete(mutex_);
    }
}

esp_err_t BleServer::init() {
    if (initialized_) {
        ESP_LOGW(TAG, "BLE already initialized");
        return ESP_OK;
    }
    
    instance_ = this;
    
    esp_err_t ret = esp_bt_controller_init(nullptr);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "BT controller init failed: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "BT controller enable failed: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ret = esp_bluedroid_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Bluedroid init failed: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ret = esp_bluedroid_enable();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Bluedroid enable failed: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ret = esp_ble_gatts_register_callback(gatts_event_handler);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "GATTS register callback failed: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "GAP register callback failed: %s", esp_err_to_name(ret));
        return ret;
    }
    
    initialized_ = true;
    ESP_LOGI(TAG, "BLE initialized");
    return ESP_OK;
}

esp_err_t BleServer::start() {
    if (!initialized_) {
        ESP_LOGE(TAG, "BLE not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    
    if (started_) {
        return ESP_OK;
    }
    
    register_app();
    
    started_ = true;
    ESP_LOGI(TAG, "BLE started");
    return ESP_OK;
}

esp_err_t BleServer::stop() {
    if (!started_) {
        return ESP_OK;
    }
    
    if (connected_) {
        esp_ble_gatts_close(service_handle_, connection_id_);
    }
    
    if (service_handle_ != 0) {
        esp_ble_gatts_delete_service(service_handle_);
    }
    
    esp_bluedroid_disable();
    esp_bluedroid_deinit();
    esp_bt_controller_disable();
    esp_bt_controller_deinit();
    
    started_ = false;
    connected_ = false;
    
    ESP_LOGI(TAG, "BLE stopped");
    return ESP_OK;
}

esp_err_t BleServer::update_location(const BleLocationData& location) {
    if (!connected_) {
        return ESP_ERR_INVALID_STATE;
    }
    
    if (xSemaphoreTake(mutex_, pdMS_TO_TICKS(100)) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }
    current_location_ = location;
    xSemaphoreGive(mutex_);
    
    return ESP_OK;
}

esp_err_t BleServer::set_device_name(const char* name) {
    if (name == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    
    if (xSemaphoreTake(mutex_, pdMS_TO_TICKS(100)) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }
    
    size_t len = strlcpy(device_name_, name, BLE_DEVICE_NAME_MAX);
    if (len >= BLE_DEVICE_NAME_MAX) {
        ESP_LOGW(TAG, "Device name truncated to %d characters", BLE_DEVICE_NAME_MAX - 1);
    }
    
    xSemaphoreGive(mutex_);
    
    if (started_) {
        esp_ble_gap_set_device_name(device_name_);
    }
    
    return ESP_OK;
}

bool BleServer::is_connected() const {
    return connected_;
}

void BleServer::register_app() {
    esp_err_t ret = esp_ble_gatts_app_register(0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "GATTS app register failed: %s", esp_err_to_name(ret));
    }
}

void BleServer::create_services() {
    esp_gatt_srvc_id_t svc_id = {
        .id = {
            .uuid = {
                .len = ESP_UUID_LEN_16,
                .uuid = {.uuid16 = BLE_SERVICE_UUID},
            },
            .inst_id = 0,
        },
        .is_primary = true,
    };
    
    esp_err_t ret = esp_ble_gatts_create_service(0, &svc_id, 4);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Create service failed: %s", esp_err_to_name(ret));
    }
}

void BleServer::add_characteristics() {
    esp_bt_uuid_t loc_uuid = {
        .len = ESP_UUID_LEN_16,
        .uuid = {.uuid16 = BLE_CHAR_LOCATION_UUID},
    };
    
    esp_attr_value_t attr_val = {};
    esp_attr_control_t attr_ctrl = {};
    
    esp_err_t ret = esp_ble_gatts_add_char(service_handle_, &loc_uuid,
                                            ESP_GATT_PERM_READ,
                                            ESP_GATT_CHAR_PROP_BIT_READ,
                                            &attr_val, &attr_ctrl);
    (void)ret;
    
    esp_bt_uuid_t name_uuid = {
        .len = ESP_UUID_LEN_16,
        .uuid = {.uuid16 = BLE_CHAR_NAME_UUID},
    };
    
    ret = esp_ble_gatts_add_char(service_handle_, &name_uuid,
                                  ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                                  ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE,
                                  &attr_val, &attr_ctrl);
    (void)ret;
}

void BleServer::start_advertising() {
    esp_ble_adv_data_t adv_data = {
        .set_scan_rsp = false,
        .include_name = true,
        .include_txpower = true,
        .min_interval = BLE_MIN_INTERVAL_MS,
        .max_interval = BLE_MAX_INTERVAL_MS,
        .appearance = 0x00,
        .manufacturer_len = 0,
        .p_manufacturer_data = nullptr,
        .service_data_len = 0,
        .p_service_data = nullptr,
        .service_uuid_len = sizeof(BLE_SERVICE_UUID),
        .p_service_uuid = (uint8_t*)&BLE_SERVICE_UUID,
        .flag = ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT,
    };
    
    esp_err_t ret = esp_ble_gap_config_adv_data(&adv_data);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Config adv data failed: %s", esp_err_to_name(ret));
        return;
    }
    
    esp_ble_adv_params_t adv_params = {
        .adv_int_min = BLE_MIN_INTERVAL_MS,
        .adv_int_max = BLE_MAX_INTERVAL_MS,
        .adv_type = ADV_TYPE_IND,
        .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
        .peer_addr = {0},
        .peer_addr_type = BLE_ADDR_TYPE_PUBLIC,
        .channel_map = ADV_CHNL_ALL,
        .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
    };
    
    ret = esp_ble_gap_start_advertising(&adv_params);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Start advertising failed: %s", esp_err_to_name(ret));
    }
}

void BleServer::send_location_response(uint16_t conn_id, uint16_t trans_id, uint16_t handle) {
    if (xSemaphoreTake(mutex_, pdMS_TO_TICKS(100)) != pdTRUE) {
        return;
    }
    
    uint8_t loc_data[sizeof(BleLocationData)];
    memcpy(loc_data, &current_location_, sizeof(loc_data));
    
    xSemaphoreGive(mutex_);
    
    esp_gatt_rsp_t rsp = {};
    rsp.attr_value.len = sizeof(loc_data);
    rsp.attr_value.handle = handle;
    memcpy(rsp.attr_value.value, loc_data, sizeof(loc_data));
    
    esp_ble_gatts_send_response(service_handle_, conn_id, trans_id, ESP_GATT_OK, &rsp);
}

void BleServer::send_name_response(uint16_t conn_id, uint16_t trans_id, uint16_t handle) {
    if (xSemaphoreTake(mutex_, pdMS_TO_TICKS(100)) != pdTRUE) {
        return;
    }
    
    size_t name_len = strlen(device_name_);
    
    xSemaphoreGive(mutex_);
    
    esp_gatt_rsp_t rsp = {};
    rsp.attr_value.len = name_len;
    rsp.attr_value.handle = handle;
    memcpy(rsp.attr_value.value, device_name_, name_len);
    
    esp_ble_gatts_send_response(service_handle_, conn_id, trans_id, ESP_GATT_OK, &rsp);
}

void BleServer::gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t* param) {
    if (instance_) {
        instance_->on_gatts_event(event, gatts_if, param);
    }
}

void BleServer::gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t* param) {
    if (instance_) {
        instance_->on_gap_event(event, param);
    }
}

void BleServer::on_gatts_event(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t* param) {
    switch (event) {
        case ESP_GATTS_REG_EVT:
            create_services();
            break;
            
        case ESP_GATTS_CREATE_EVT:
            service_handle_ = param->create.service_handle;
            add_characteristics();
            esp_ble_gatts_start_service(service_handle_);
            esp_ble_gap_set_device_name(device_name_);
            start_advertising();
            break;
            
        case ESP_GATTS_START_EVT:
            ESP_LOGI(TAG, "Service started");
            break;
            
        case ESP_GATTS_CONNECT_EVT:
            connection_id_ = param->connect.conn_id;
            connected_ = true;
            ESP_LOGI(TAG, "Client connected");
            break;
            
        case ESP_GATTS_DISCONNECT_EVT:
            connected_ = false;
            connection_id_ = 0;
            ESP_LOGI(TAG, "Client disconnected, restarting advertising");
            start_advertising();
            break;
            
        case ESP_GATTS_READ_EVT:
            if (param->read.handle == location_char_handle_) {
                send_location_response(param->read.conn_id, param->read.trans_id, param->read.handle);
            } else if (param->read.handle == name_char_handle_) {
                send_name_response(param->read.conn_id, param->read.trans_id, param->read.handle);
            }
            break;
            
        case ESP_GATTS_WRITE_EVT:
            if (param->write.handle == name_char_handle_) {
                if (xSemaphoreTake(mutex_, pdMS_TO_TICKS(100)) == pdTRUE) {
                    size_t len = param->write.len;
                    if (len >= BLE_DEVICE_NAME_MAX) {
                        len = BLE_DEVICE_NAME_MAX - 1;
                    }
                    memcpy(device_name_, param->write.value, len);
                    device_name_[len] = '\0';
                    xSemaphoreGive(mutex_);
                    ESP_LOGI(TAG, "Device name updated: %s", device_name_);
                }
            }
            break;
            
        case ESP_GATTS_ADD_CHAR_EVT:
            if (param->add_char.char_uuid.uuid.uuid16 == BLE_CHAR_LOCATION_UUID) {
                location_char_handle_ = param->add_char.attr_handle;
                ESP_LOGI(TAG, "Location characteristic added with handle %d", location_char_handle_);
            } else if (param->add_char.char_uuid.uuid.uuid16 == BLE_CHAR_NAME_UUID) {
                name_char_handle_ = param->add_char.attr_handle;
                ESP_LOGI(TAG, "Name characteristic added with handle %d", name_char_handle_);
            }
            break;
            
        default:
            break;
    }
}

void BleServer::on_gap_event(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t* param) {
    switch (event) {
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
            ESP_LOGI(TAG, "Adv data set complete");
            break;
            
        case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
            ESP_LOGI(TAG, "Scan rsp data set complete");
            break;
            
        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
            if (param->adv_start_cmpl.status == ESP_BT_STATUS_SUCCESS) {
                ESP_LOGI(TAG, "Advertising started successfully");
            } else {
                ESP_LOGE(TAG, "Advertising start failed");
            }
            break;
            
        case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
            if (param->adv_stop_cmpl.status == ESP_BT_STATUS_SUCCESS) {
                ESP_LOGI(TAG, "Advertising stopped");
            }
            break;
            
        default:
            break;
    }
}
