#include "HomePage.hpp"

LV_IMG_DECLARE(img_app_homepage);
// 设置应用图标
HomePage::HomePage() : ESP_Brookesia_PhoneApp("实验", &img_app_homepage, true) // 屏幕对象数组初始化为空
{
}

HomePage::~HomePage()
{
}

bool HomePage::run(void)
{
    // 激活时刷新数据
    return true;
}

bool HomePage::back(void)
{
    // 激活时刷新数据
    return true;
}

bool HomePage::close(void)
{
    // 激活时刷新数据
    _is_ui_del = true;
    return true;
}

esp_err_t HomePage::initUi()
{

    return ESP_OK;
}

/* 暂停应用（保留状态） */
bool HomePage::pause(void)
{
    _is_ui_resumed = true; // 标记UI处于暂停状态
    return true;
}

/* 恢复应用 */
bool HomePage::resume(void)
{
    _is_ui_resumed = false; // 清除暂停状态
    return true;
}
bool HomePage::init(void)
{
    return true;
}
