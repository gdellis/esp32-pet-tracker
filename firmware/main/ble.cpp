#include "ble.hpp"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_gatts_attr_db.h"
#include "esp_gap_ble_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_defs.h"

static const char* TAG = "ble";

BleServer* BleServer::instance_ = nullptr;

BleServer::BleServer() = default;

esp_err_t BleServer::init() {
    if (started_) {
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
    
    register_app();
    
    ESP_LOGI(TAG, "BLE initialized");
    return ESP_OK;
}

esp_err_t BleServer::start() {
    if (started_) {
        return ESP_OK;
    }
    
    esp_err_t ret = esp_ble_gatts_app_register(0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "GATTS app register failed: %s", esp_err_to_name(ret));
        return ret;
    }
    
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
    current_location_ = location;
    return ESP_OK;
}

esp_err_t BleServer::set_device_name(const char* name) {
    if (name == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    
    size_t len = 0;
    while (name[len] != '\0' && len < BLE_DEVICE_NAME_MAX - 1) {
        len++;
    }
    
    memcpy(device_name_, name, len);
    device_name_[len] = '\0';
    
    if (started_) {
        esp_ble_gap_set_device_name(device_name_);
    }
    
    return ESP_OK;
}

void BleServer::register_app() {
    ESP_LOGI(TAG, "Registering GATT application");
}

void BleServer::create_services() {
    esp_gatt_srvc_id_t svc_id = {
        .id = {
            .uuid = {
                .len = ESP_UUID_TYPE_16,
                .uuid = {.uuid16 = BLE_SERVICE_UUID},
            },
            .inst_id = 0,
        },
        .is_primary = true,
    };
    
    esp_err_t ret = esp_ble_gatts_create_service(service_handle_, &svc_id, 4);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Create service failed: %s", esp_err_to_name(ret));
        return;
    }
}

void BleServer::send_location_response(uint16_t conn_id, uint16_t trans_id) {
    uint8_t loc_data[sizeof(BleLocationData)];
    memcpy(loc_data, &current_location_, sizeof(loc_data));
    
    esp_gatt_rsp_t rsp = {
        .attr_value = {
            .len = sizeof(loc_data),
            .handle = location_char_handle_,
            .value = {},
        },
    };
    memcpy(rsp.attr_value.value, loc_data, sizeof(loc_data));
    
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
            if (param->reg.app_id == 0) {
                service_handle_ = param->reg.service_handle;
                create_services();
                esp_ble_gap_set_device_name(device_name_);
            }
            break;
            
        case ESP_GATTS_CREATE_EVT:
            service_handle_ = param->create.service_handle;
            esp_ble_gatts_start_service(service_handle_);
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
            ESP_LOGI(TAG, "Client disconnected");
            break;
            
        case ESP_GATTS_READ_EVT:
            if (param->read.handle == location_char_handle_) {
                send_location_response(param->read.conn_id, param->read.trans_id);
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
                ESP_LOGI(TAG, "Advertising started");
            }
            break;
            
        default:
            break;
    }
}
