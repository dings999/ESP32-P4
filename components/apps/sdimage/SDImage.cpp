#include "SDImage.hpp"
#include "ui/sdimage_ui.h"     // SquareLine生成的LVGL界面定义

LV_IMG_DECLARE(img_app_sdimage);
// 设置应用图标
SDImage::SDImage() : ESP_Brookesia_PhoneApp("SD卡", &img_app_sdimage, true) // 屏幕对象数组初始化为空
{
}

SDImage::~SDImage()
{
}

bool SDImage::run(void)
{
    ui_sdimage_init(); // 初始化SquareLine生成的UI
    // 激活时刷新数据
    return true;
}

bool SDImage::back(void)
{
    // 激活时刷新数据
    return true;
}

bool SDImage::close(void)
{
    // 激活时刷新数据
    _is_ui_del = true;
    return true;
}

esp_err_t SDImage::initUi()
{

    return ESP_OK;
}

/* 暂停应用（保留状态） */
bool SDImage::pause(void)
{
    _is_ui_resumed = true; // 标记UI处于暂停状态
    return true;
}

/* 恢复应用 */
bool SDImage::resume(void)
{
    _is_ui_resumed = false; // 清除暂停状态
    return true;
}
bool SDImage::init(void)
{
    return true;
}
