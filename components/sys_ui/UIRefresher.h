#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_log.h"
#include "WiFiManager.hpp"
#include "esp_brookesia.hpp"

class UIRefresher {
public:
    // 获取单例实例
    static UIRefresher& getInstance();
    
    // 初始化刷新器
    void init(ESP_Brookesia_Phone* phone);
    
    // 设置刷新间隔（毫秒）
    void setRefreshInterval(uint32_t interval_ms);
    
    // 启用/禁用特定功能
    void enableTimeRefresh(bool enable);
    void enableWifiRefresh(bool enable);
    void enableMemoryRefresh(bool enable);
    
private:
    // 私有构造函数
    UIRefresher();
    
    // 实际刷新任务
    static void refreshTask(void* arg);
    
    // 各个刷新功能
    void refreshTime();
    void refreshWifiStatus();
    void refreshMemoryUsage();
    
    // 成员变量
    ESP_Brookesia_Phone* m_phone;
    TaskHandle_t m_taskHandle;
    uint32_t m_refreshInterval;
    bool m_timeRefreshEnabled;
    bool m_wifiRefreshEnabled;
    bool m_memoryRefreshEnabled;
};
