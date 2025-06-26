#include "UIRefresher.h"
#include "bsp/esp-bsp.h"            // 板级支持包（显示屏/背光控制）

UIRefresher& UIRefresher::getInstance() {
    static UIRefresher instance;
    return instance;
}

UIRefresher::UIRefresher() 
    : m_phone(nullptr),
      m_taskHandle(nullptr),
      m_refreshInterval(2000),
      m_timeRefreshEnabled(true),
      m_wifiRefreshEnabled(true),
      m_memoryRefreshEnabled(false) {}

void UIRefresher::init(ESP_Brookesia_Phone* phone) {
    if(!phone) {
        ESP_LOGE("UIRefresher", "Invalid phone instance");
        return;
    }
    
    m_phone = phone;
    
    // 创建刷新任务
    xTaskCreate(
        refreshTask,
        "UI Refresher",
        4096,  // 堆栈大小
        this,  // 传递this指针
        2,     // 优先级
        &m_taskHandle
    );
}

void UIRefresher::setRefreshInterval(uint32_t interval_ms) {
    m_refreshInterval = interval_ms;
}

void UIRefresher::enableTimeRefresh(bool enable) {
    m_timeRefreshEnabled = enable;
}

void UIRefresher::enableWifiRefresh(bool enable) {
    m_wifiRefreshEnabled = enable;
}

void UIRefresher::enableMemoryRefresh(bool enable) {
    m_memoryRefreshEnabled = enable;
}

void UIRefresher::refreshTask(void* arg) {
    UIRefresher* instance = static_cast<UIRefresher*>(arg);
    const TickType_t xFrequency = pdMS_TO_TICKS(instance->m_refreshInterval);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    while(1) {
        if(instance->m_timeRefreshEnabled) {
            instance->refreshTime();
        }
        
        if(instance->m_wifiRefreshEnabled) {
            instance->refreshWifiStatus();
        }
        
        if(instance->m_memoryRefreshEnabled) {
            instance->refreshMemoryUsage();
        }
        
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void UIRefresher::refreshTime() {
    if(!m_phone || !m_phone->getHome().getStatusBar()) return;
    
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    
    bool is_pm = (timeinfo.tm_hour >= 12);
    uint8_t hour_12 = timeinfo.tm_hour % 12;
    hour_12 = hour_12 ? hour_12 : 12; // 处理0点
    
    if(bsp_display_lock(50)) { // 50ms超时
        m_phone->getHome().getStatusBar()->setClock(
            hour_12,
            timeinfo.tm_min,
            is_pm
        );
        bsp_display_unlock();
    }
}



void UIRefresher::refreshWifiStatus() {
    if (!m_phone) return;
    
    auto status_bar = m_phone->getHome().getStatusBar();
    int level = WiFiManager::getInstance().getSignalLevel();
    
    if (bsp_display_lock(50)) {
        status_bar->setWifiIconState(level);
        bsp_display_unlock();
    }
}


void UIRefresher::refreshMemoryUsage() {
    if(!m_phone || !m_phone->getHome().getRecentsScreen()) return;
    
    uint32_t free_sram = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    uint32_t total_sram = heap_caps_get_total_size(MALLOC_CAP_INTERNAL);
    
    if(bsp_display_lock(50)) {
        m_phone->getHome().getRecentsScreen()->setMemoryLabel(
            free_sram / 1024,
            total_sram / 1024,
            0, 0  // PSRAM数据可根据需要添加
        );
        bsp_display_unlock();
    }
}
