#include "KeyTools.hpp"
#include "ui/keytools_ui.h"     // SquareLine生成的LVGL界面定义
#include "tinyusb.h"
#include "esp_log.h"
static const char *TAG = "KeyTools";

LV_IMG_DECLARE(img_app_keytools);
// 设置应用图标
KeyTools::KeyTools() : ESP_Brookesia_PhoneApp("键盘工具", &img_app_keytools, true) // 屏幕对象数组初始化为空
{
}

KeyTools::~KeyTools()
{
}

bool KeyTools::run(void)
{
    ui_keytools_init(); // 初始化SquareLine生成的UI
    // 激活时刷新数据
    return true;
}

bool KeyTools::back(void)
{
    // 激活时刷新数据
    return true;
}

bool KeyTools::close(void)
{
    esp_err_t ret = tinyusb_driver_uninstall();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to uninstall USB driver (%s)", esp_err_to_name(ret));
    }
    ESP_LOGI(TAG, "KeyTools closed");
    _is_ui_del = true;
    return true;
}

esp_err_t KeyTools::initUi()
{

    return ESP_OK;
}

/* 暂停应用（保留状态） */
bool KeyTools::pause(void)
{
    _is_ui_resumed = true; // 标记UI处于暂停状态
    return true;
}

/* 恢复应用 */
bool KeyTools::resume(void)
{
    _is_ui_resumed = false; // 清除暂停状态
    return true;
}
bool KeyTools::init(void)
{
    return true;
}
