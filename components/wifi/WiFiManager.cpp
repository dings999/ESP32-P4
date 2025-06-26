#include "WiFiManager.hpp"
#include "esp_log.h"
#include "esp_check.h"
#include <cstring>
#include "esp_netif.h"
#include "setting/app_sntp.h"
static const char* TAG = "WiFiManager";

typedef enum {
    WIFI_EVENT_CONNECTED = BIT(0),  // 连接成功标志位
    WIFI_EVENT_INIT_DONE = BIT(1),  // WiFi初始化完成标志
    WIFI_EVENT_UI_INIT_DONE = BIT(2), // UI初始化完成标志
    WIFI_EVENT_SCANING = BIT(3)     // 扫描中标志
} wifi_event_id_t;

WiFiManager& WiFiManager::getInstance() {
    static WiFiManager instance;
    return instance;
}

WiFiManager::WiFiManager() {
    WiFiManager::init();
    if (!WiFiManager::autoConnect()) {
        ESP_LOGI(TAG, "No saved WiFi credentials, skip auto connect");
    }
}

WiFiManager::~WiFiManager() {
    vEventGroupDelete(wifi_event_group);
}

bool WiFiManager::init() {
    ESP_LOGI(TAG, "Initializing WiFi...");

      wifi_event_group = xEventGroupCreate(); // 创建事件组
 

    // 网络协议栈初始化
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta(); // 创建STA接口
    assert(sta_netif);

    // WiFi驱动初始化
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // 注册全局WiFi事件处理器
    esp_event_handler_instance_t instance_any_id;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifiEventHandler, this, &instance_any_id));

    // 启动WiFi
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    // startSignalMonitor();
    return true;
}


bool WiFiManager::connect(const char* ssid, const char* password, ConnectCallback callback) {
    if (!ssid || !password) {
        ESP_LOGE(TAG, "Invalid SSID or password");
        return false;
    }

    ConnectContext* ctx = new ConnectContext{callback, ssid, password};
    xTaskCreate(connectTask, "wifi_connect", 4096, ctx, 5, nullptr);
    return true;
}

bool WiFiManager::autoConnect(ConnectCallback callback) {
    char ssid[32] = {0};
    char password[64] = {0};
    
    if (!loadCredentials(ssid, password)) {
        ESP_LOGE(TAG, "No saved WiFi credentials");
        return false;
    }
    
    return connect(ssid, password, callback);
}

bool WiFiManager::isConnected() {
    return (xEventGroupGetBits(wifi_event_group) & BIT0) != 0;
}

void WiFiManager::disconnect() {
    esp_wifi_disconnect();
    xEventGroupClearBits(wifi_event_group, BIT0);
}

bool WiFiManager::startScan(ScanCallback callback) {
    ScanContext* ctx = new ScanContext;
    ctx->callback = callback;
    
    xTaskCreate(scanTask, "wifi_scan", 6144, ctx, 2, nullptr);
    return true;
}

bool WiFiManager::getCurrentApInfo(wifi_ap_record_t* ap_info) {
    return esp_wifi_sta_get_ap_info(ap_info) == ESP_OK;
}

WiFiManager::SignalLevel WiFiManager::getSignalLevel() {
    wifi_ap_record_t ap_info;
    if (!getCurrentApInfo(&ap_info)) return SIGNAL_NONE;
    
    if (ap_info.rssi > -60) return SIGNAL_GOOD;
    if (ap_info.rssi > -80) return SIGNAL_MODERATE;
    if (ap_info.rssi > -100) return SIGNAL_WEAK;
    return SIGNAL_NONE;
}

int WiFiManager::getRssi() {
    wifi_ap_record_t ap_info;
    if (!getCurrentApInfo(&ap_info)) return 0;
    return ap_info.rssi;
}

void WiFiManager::wifiEventHandler(void* arg, esp_event_base_t event_base, 
                                  int32_t event_id, void* event_data) {
    WiFiManager* instance = static_cast<WiFiManager*>(arg);

    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_CONNECTED:
                xEventGroupSetBits(instance->wifi_event_group, BIT0);
                 app_sntp_init(); 
                ESP_LOGI(TAG, "Connected to AP");
                if (instance->status_callback) {
                    instance->status_callback(true);
                }
                break;
            case WIFI_EVENT_STA_DISCONNECTED: {
                xEventGroupClearBits(instance->wifi_event_group, BIT0);
                if (instance->status_callback) {
                    instance->status_callback(false);
                }
                break;
            }
            case WIFI_EVENT_SCAN_DONE:
                ESP_LOGI(TAG, "WiFi scan completed");
                break;
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(instance->wifi_event_group, BIT0);
        ESP_LOGI(TAG, "Got IP address");
    }
}

void WiFiManager::scanTask(void* arg) {
    ScanContext* ctx = static_cast<ScanContext*>(arg);
    uint16_t ap_count = 0;
    std::vector<wifi_ap_record_t> ap_records;
  
     WiFiManager& instance = getInstance(); // 获取类实例
    
    // 设置扫描状态
    instance.scanning_in_progress = true;

    esp_wifi_scan_start(nullptr, true);
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
    
    if (ap_count > 0) {
        ap_records.resize(ap_count);
        uint16_t number = ap_count;
        ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_records.data()));
    }
    instance.scanning_in_progress = false;
    if (ctx->callback) {
        ctx->callback(ap_records);
    }
    
    delete ctx;
    vTaskDelete(NULL);
}



void WiFiManager::connectTask(void* arg) {
    ConnectContext* ctx = static_cast<ConnectContext*>(arg);
    wifi_config_t wifi_config = {0};
    
    strncpy((char*)wifi_config.sta.ssid, ctx->ssid.c_str(), sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char*)wifi_config.sta.password, ctx->password.c_str(), sizeof(wifi_config.sta.password) - 1);
    
    esp_wifi_disconnect();
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    esp_wifi_connect();
    
    // 等待连接结果
    EventBits_t bits = xEventGroupWaitBits(WiFiManager::getInstance().wifi_event_group,
                                          BIT0, pdFALSE, pdFALSE, pdMS_TO_TICKS(10000));
    
    if (ctx->callback) {
        ctx->callback((bits & BIT0) != 0);
    }
    
    delete ctx;
    vTaskDelete(NULL);
}

bool WiFiManager::saveCredentials(const char* ssid, const char* password) {
    nvs_handle_t nvs_handle;
    esp_err_t err;
    bool success = true;
    
    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) return false;

    err = nvs_set_str(nvs_handle, NVS_KEY_SSID, ssid);
    if (err != ESP_OK) success = false;

    err = nvs_set_str(nvs_handle, NVS_KEY_PWD, password);
    if (err != ESP_OK) success = false;

    if (success) {
        nvs_commit(nvs_handle);
        strncpy(saved_ssid, ssid, sizeof(saved_ssid) - 1);
        strncpy(saved_password, password, sizeof(saved_password) - 1);
    }
    
    nvs_close(nvs_handle);
    return success;
}

bool WiFiManager::loadCredentials(char* ssid, char* password) {
    nvs_handle_t nvs_handle;
    size_t len = 32;
    
    if (nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs_handle) != ESP_OK)
        return false;
    
    if (nvs_get_str(nvs_handle, NVS_KEY_SSID, ssid, &len) != ESP_OK) {
        nvs_close(nvs_handle);
        return false;
    }
    
    len = 64;
    if (nvs_get_str(nvs_handle, NVS_KEY_PWD, password, &len) != ESP_OK) {
        nvs_close(nvs_handle);
        return false;
    }
    
    nvs_close(nvs_handle);
    return true;
}



WiFiManager::SignalLevel WiFiManager::getSignalLevel(int rssi) {
    if (rssi > -60) return SIGNAL_GOOD;
    if (rssi > -80) return SIGNAL_MODERATE;
    if (rssi > -100) return SIGNAL_WEAK;
    return SIGNAL_NONE;
}

bool WiFiManager::isConnectedTo(const char* ssid) {
    if (!isConnected()) {
        return false;
    }
    
    wifi_ap_record_t ap_info;
    if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
        return strncmp((const char*)ap_info.ssid, ssid, sizeof(ap_info.ssid)) == 0;
    }
    return false;
}

// void WiFiManager::registerStatusCallback(StatusCallback callback) {
//     status_callback = callback;
// }

// void WiFiManager::registerSignalStrengthCallback(SignalStrengthCallback callback) {
//     _signal_strength_cb = callback;
// }

// void WiFiManager::startSignalMonitor() {
//     if (!_signal_monitor_task) {
//         xTaskCreate(
//             signalMonitorTask,
//             "wifi_signal_monitor",
//             2048,  // 足够监控任务的堆栈
//             this,  // 传递this指针
//             2,     // 中等优先级
//             &_signal_monitor_task
//         );
//     }
// }

// void WiFiManager::signalMonitorTask(void* arg) {
//     WiFiManager* instance = static_cast<WiFiManager*>(arg);
//     int last_level = -1; // 初始化为无效值
    
//     while (true) {
//         if (instance->isConnected()) {
//             ESP_LOGI(TAG, "WiFi signalMonitorTask");
//             wifi_ap_record_t ap_info;
//             if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
//                 // 计算当前信号等级
//                 int current_level = 0;
//                 if (ap_info.rssi > -60) current_level = 3;
//                 else if (ap_info.rssi > -80) current_level = 2;
//                 else if (ap_info.rssi > -100) current_level = 1;
//                 ESP_LOGI(TAG, "WiFi signalMonitorTask %d", current_level);
//                 // 只有等级变化时才回调
//                 if (current_level != last_level && instance->_signal_strength_cb) {
//                     instance->_signal_strength_cb(current_level);
//                     last_level = current_level;
//                     ESP_LOGI(TAG, "WiFi signalMonitorTask %d", current_level);
//                 }
//             }
//         } else if (last_level != 0) { // 断开连接状态
//             if (instance->_signal_strength_cb) {
//                 instance->_signal_strength_cb(0);
//             }
//             last_level = 0;
//         }
        
//         vTaskDelay(pdMS_TO_TICKS(5000)); // 3秒检测间隔
//     }
// }

