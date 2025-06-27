#include <array>
#include <map>
#include <string>
#include "esp_event.h"
#include "lvgl.h"
#include "esp_brookesia.hpp"

class HomePage : public ESP_Brookesia_PhoneApp
{
public:
    HomePage();
    virtual ~HomePage();

    bool run(void);
    bool back(void);
    bool close(void);

    bool init(void) override;
    bool pause(void) override;
    bool resume(void) override;

private:
    typedef enum
    {
        UI_MAIN_SETTING_INDEX = 0,
        UI_WIFI_SCAN_INDEX,
        UI_WIFI_CONNECT_INDEX,
        UI_BLUETOOTH_SETTING_INDEX,
        UI_VOLUME_SETTING_INDEX,
        UI_BRIGHTNESS_SETTING_INDEX,
        UI_ABOUT_SETTING_INDEX,
        UI_MAX_INDEX,
    } SettingScreenIndex_t;
    // UI初始化
    esp_err_t initUi();
    bool _is_ui_resumed;
    bool _is_ui_del;
    SettingScreenIndex_t _screen_index;
    std::array<lv_obj_t *, UI_MAX_INDEX> _screen_list;
};
