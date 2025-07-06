#include "ImageGet.hpp"
#include "ui/imageget_ui.h"     // SquareLine生成的LVGL界面定义

LV_IMG_DECLARE(img_app_image);
// 设置应用图标
ImageGet::ImageGet() : ESP_Brookesia_PhoneApp("图像获取", &img_app_image, true) // 屏幕对象数组初始化为空
{
}

ImageGet::~ImageGet()
{
}

bool ImageGet::run(void)
{
    ui_imageget_init(); // 初始化SquareLine生成的UI
    // 激活时刷新数据
    return true;
}

bool ImageGet::back(void)
{
    // 激活时刷新数据
    return true;
}

bool ImageGet::close(void)
{
    // 激活时刷新数据
    _is_ui_del = true;
    return true;
}

esp_err_t ImageGet::initUi()
{

    return ESP_OK;
}

/* 暂停应用（保留状态） */
bool ImageGet::pause(void)
{
    _is_ui_resumed = true; // 标记UI处于暂停状态
    return true;
}

/* 恢复应用 */
bool ImageGet::resume(void)
{
    _is_ui_resumed = false; // 清除暂停状态
    return true;
}
bool ImageGet::init(void)
{
    return true;
}
