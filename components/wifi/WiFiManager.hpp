#pragma once
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "freertos/event_groups.h"
#include <functional>
#include <vector>
#include <string>

class WiFiManager {
public:
    using StatusCallback = std::function<void(bool connected)>;
    using ScanCallback = std::function<void(std::vector<wifi_ap_record_t>&)>;
    using ConnectCallback = std::function<void(bool success)>;

    enum SignalLevel {
        SIGNAL_NONE = 0,
        SIGNAL_WEAK,
        SIGNAL_MODERATE,
        SIGNAL_GOOD
    };
    EventGroupHandle_t getEventGroup() { return wifi_event_group; }
    static WiFiManager& getInstance();
    
    bool init();
    bool connect(const char* ssid, const char* password, ConnectCallback callback = nullptr);
    bool autoConnect(ConnectCallback callback = nullptr);
    bool isConnected();
    void disconnect();
    SignalLevel getSignalLevel();
    SignalLevel getSignalLevel(int rssi);
    bool startScan(ScanCallback callback = nullptr);
    bool getCurrentApInfo(wifi_ap_record_t* ap_info);

    int getRssi();
    bool isConnectedTo(const char* ssid);
    bool saveCredentials(const char* ssid, const char* password);
    bool loadCredentials(char* ssid, char* password);
    
    void registerStatusCallback(StatusCallback callback);
    bool isScanning() const { return scanning_in_progress; }
private:
    struct ScanContext {
        ScanCallback callback;
        std::vector<wifi_ap_record_t> results;
    };
    
    struct ConnectContext {
        ConnectCallback callback;
        std::string ssid;
        std::string password;
    };
    
    WiFiManager();
    ~WiFiManager();
    bool scanning_in_progress = false;
    static void wifiEventHandler(void* arg, esp_event_base_t event_base, 
                               int32_t event_id, void* event_data);
    static void scanTask(void* arg);
    static void connectTask(void* arg);
    
    StatusCallback status_callback;
    EventGroupHandle_t wifi_event_group;
    char saved_ssid[32];
    char saved_password[64];
    
    static constexpr auto NVS_NAMESPACE = "storage";
    static constexpr auto NVS_KEY_SSID = "ssid";
    static constexpr auto NVS_KEY_PWD = "pwd";


};