/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* 头文件引入 */
// #include <cmath>                    // 数学函数库（未实际使用）
#include "freertos/FreeRTOS.h"      // FreeRTOS核心功能（任务/队列/信号量）
#include "freertos/task.h"          // FreeRTOS任务管理API
#include "esp_log.h"                // ESP-IDF日志系统（ESP_LOGI等）
#include "esp_err.h"                // ESP错误码定义（ESP_OK, ESP_FAIL等）
#include "esp_wifi.h"               // ESP32 WiFi驱动
#include "esp_check.h"              // ESP错误检查宏（ESP_GOTO_ON_ERROR等）
#include "esp_memory_utils.h"       // ESP内存操作工具（heap_caps）
#include "esp_mac.h"                // MAC地址操作接口
#include "bsp/esp-bsp.h"            // 板级支持包（显示屏/背光控制）
#include "bsp_board_extra.h"            // 板级支持包（显示屏/背光控制）
#include "nvs.h"                    // 非易失存储(NVS)读写接口
#include "WiFiManager.hpp"
/* 自定义头文件 */
#include "ui/ui.h"                  // SquareLine生成的LVGL界面定义
#include "Setting.hpp"              // AppSettings类声明
//#include "app_sntp.h"               // SNTP时间同步模块
#include "esp_brookesia_versions.h" // 版本信息头文件

/* SPDX版权声明和许可证 */

#define WIFI_CONNECT_UI_WAIT_TIME_MS (1 * 1000)  // 连接成功/失败后UI显示时间（1000ms）
#define WIFI_CONNECT_RET_WAIT_TIME_MS (10 * 1000) // 连接超时时间（10000ms）

/* 硬件参数范围 */
#define SCREEN_BRIGHTNESS_MIN (20)  // 屏幕亮度最小值
#define SCREEN_BRIGHTNESS_MAX (BSP_LCD_BACKLIGHT_BRIGHTNESS_MAX) // 亮度最大值（硬件定义）
#define SPEAKER_VOLUME_MIN (0)      // 扬声器音量最小值
#define SPEAKER_VOLUME_MAX (100)    // 音量最大值

/* NVS存储键名 */
#define NVS_STORAGE_NAMESPACE "storage"        // NVS命名空间名称
#define NVS_KEY_WIFI_ENABLE "wifi_en"          // WiFi开关状态键
#define NVS_KEY_BLE_ENABLE "ble_en"             // 蓝牙开关状态键
#define NVS_KEY_AUDIO_VOLUME "volume"           // 音频音量键
#define NVS_KEY_DISPLAY_BRIGHTNESS "brightness" // 屏幕亮度键

#define NVS_KEY_WIFI_SSID "ssid" // WIFI SSID
#define NVS_KEY_WIFI_PWD "pwd" //   WIFI 密码

/* UI布局参数 */
#define UI_MAIN_ITEM_LEFT_OFFSET (20)         // 主界面左侧偏移（像素）
#define UI_WIFI_LIST_UP_OFFSET (20)           // WiFi列表上方偏移（像素）
#define UI_WIFI_LIST_UP_PAD (20)              // WiFi列表顶部内边距
#define UI_WIFI_LIST_DOWN_PAD (20)             // WiFi列表底部内边距
#define UI_WIFI_LIST_H_PERCENT (75)            // WiFi列表高度百分比
#define UI_WIFI_LIST_ITEM_H (60)               // WiFi列表项高度（像素）
#define UI_WIFI_LIST_ITEM_FONT (&lv_font_montserrat_26) // WiFi列表字体
#define UI_WIFI_KEYBOARD_H_PERCENT (30)        // 键盘高度百分比
#define UI_WIFI_ICON_LOCK_RIGHT_OFFSET (-10)   // 锁图标右偏移（像素）
#define UI_WIFI_ICON_SIGNAL_RIGHT_OFFSET (-50) // 信号图标右偏移（像素）
#define UI_WIFI_ICON_CONNECT_RIGHT_OFFSET (-90) // 连接图标右偏移（像素）

/* 其他常量 */
#define SCAN_LIST_SIZE 25  // WiFi扫描结果最大数量
using namespace std;       // 使用std命名空间

/* 日志标签 */
static const char TAG[] = "EUI_Setting";  // 日志前缀标识


/* MAC地址 */
static uint8_t base_mac_addr[6] = {0};  // 存储MAC地址（二进制）
static char mac_str[18] = {0};          // MAC地址字符串形式

/* WiFi列表UI元素数组 */
static lv_obj_t* panel_wifi_btn[SCAN_LIST_SIZE];      // WiFi列表按钮容器
static lv_obj_t* label_wifi_ssid[SCAN_LIST_SIZE];     // WiFi名称标签
static lv_obj_t* img_img_wifi_lock[SCAN_LIST_SIZE];  // WiFi加密锁图标
static lv_obj_t* wifi_image[SCAN_LIST_SIZE];         // WiFi信号强度图标
static lv_obj_t* wifi_connect[SCAN_LIST_SIZE];       // WiFi连接状态图标

/* 当前亮度值 */
 static int brightness;  // 屏幕亮度缓存值

/* LVGL图像声明（外部资源） */
LV_IMG_DECLARE(img_wifisignal_absent);   // 无信号图标
LV_IMG_DECLARE(img_wifisignal_wake);     // 弱信号图标
LV_IMG_DECLARE(img_wifisignal_moderate); // 中等信号图标
LV_IMG_DECLARE(img_wifisignal_good);     // 强信号图标
LV_IMG_DECLARE(img_wifi_lock);           // 加密锁图标
LV_IMG_DECLARE(img_wifi_connect_success); // 连接成功图标
LV_IMG_DECLARE(img_wifi_connect_fail);   // 连接失败图标

/* WiFi事件标志枚举 */
typedef enum {
    WIFI_EVENT_CONNECTED = BIT(0),  // 连接成功标志位
    WIFI_EVENT_SCANING = BIT(3)     // 扫描中标志
} wifi_event_id_t;

/* 外部UI元素声明 */
LV_IMG_DECLARE(img_app_setting);  // 设置应用图标
extern lv_obj_t *ui_Min;  // 分钟显示标签（外部定义）
extern lv_obj_t *ui_Hour; // 小时显示标签
extern lv_obj_t *ui_Sec;  // 秒显示标签
extern lv_obj_t *ui_Date; // 日期显示标签
extern lv_obj_t *ui_Clock_Number; // 时钟数字容器


EventGroupHandle_t s_wifi_event_group; 

/* 构造函数 */
AppSettings::AppSettings():
    ESP_Brookesia_PhoneApp("Settings", &img_app_setting, true), // 基类构造：应用名/图标/自动调整可视区
    _is_ui_resumed(false),     // UI恢复标志初始化为false
    _is_ui_del(true),          // UI删除标志初始化为true（未创建）
    _screen_index(UI_MAIN_SETTING_INDEX), // 当前屏幕索引（主设置页）
    _screen_list({nullptr})    // 屏幕对象数组初始化为空
{
}



/* 析构函数 */
AppSettings::~AppSettings()
{
}

/* 运行应用（入口函数） */
bool AppSettings::run(void)
{   
    _is_ui_del = false;  // 标记UI已创建（未被删除）
    ui_setting_init();   // 初始化SquareLine生成的UI

    // 获取并格式化MAC地址
   char mac_str[18];
uint8_t base_mac_addr[6];
esp_read_mac(base_mac_addr, ESP_MAC_EFUSE_FACTORY);
snprintf(mac_str, sizeof(mac_str), "%02X-%02X-%02X-%02X-%02X-%02X",
         base_mac_addr[0], base_mac_addr[1], base_mac_addr[2],
         base_mac_addr[3], base_mac_addr[4], base_mac_addr[5]);

    extraUiInit();        // 初始化自定义UI组件
    updateUiByNvsParam(); // 从NVS加载参数更新UI
    return true;
}

/* 返回操作处理 */
bool AppSettings::back(void)
{
    _is_ui_resumed = false;
    
    if (_screen_index == UI_WIFI_CONNECT_INDEX) {
        lv_scr_load(ui_ScreenSettingWiFi);
    } else if (_screen_index != UI_MAIN_SETTING_INDEX) {
        lv_scr_load(ui_ScreenSettingMain);
    } else {
        while(WiFiManager::getInstance().isScanning()) {
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        notifyCoreClosed();
    }
    return true;
}

/* 关闭应用 */
bool AppSettings::close(void)
{
    while(WiFiManager::getInstance().isScanning()) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    _is_ui_del = true;
    return true;
}

/* 应用初始化 */
bool AppSettings::init(void)
{
    ESP_Brookesia_Phone *phone = getPhone();
    ESP_Brookesia_PhoneHome& home = phone->getHome();
    status_bar = home.getStatusBar();    // 状态栏对象
    backstage = home.getRecentsScreen(); // 后台管理对象
    s_wifi_event_group= WiFiManager::getInstance().getEventGroup();

    // 初始化NVS参数默认值
    _nvs_param_map[NVS_KEY_WIFI_ENABLE] = false;  // WiFi默认关闭
    _nvs_param_map[NVS_KEY_BLE_ENABLE] = false;   // 蓝牙默认关闭
    _nvs_param_map[NVS_KEY_AUDIO_VOLUME] = bsp_extra_codec_volume_get(); // 从硬件获取当前音量
    // 音量边界保护
    _nvs_param_map[NVS_KEY_AUDIO_VOLUME] = max(min((int)_nvs_param_map[NVS_KEY_AUDIO_VOLUME], SPEAKER_VOLUME_MAX), SPEAKER_VOLUME_MIN);
    // 亮度边界保护（注释掉的原始实现）
    _nvs_param_map[NVS_KEY_DISPLAY_BRIGHTNESS] = max(min((int)_nvs_param_map[NVS_KEY_DISPLAY_BRIGHTNESS], SCREEN_BRIGHTNESS_MAX), SCREEN_BRIGHTNESS_MIN);

    loadNvsParam(); // 从NVS加载实际参数
    // 应用系统参数
    bsp_extra_codec_volume_set(_nvs_param_map[NVS_KEY_AUDIO_VOLUME], (int *)&_nvs_param_map[NVS_KEY_AUDIO_VOLUME]);
    bsp_display_brightness_set(_nvs_param_map[NVS_KEY_DISPLAY_BRIGHTNESS]);


    return true;
}

void AppSettings::updateWifiIcon() {
    if (_is_ui_del) return;
    
    bsp_display_lock(0);
    switch (WiFiManager::getInstance().getSignalLevel()) {
        case WiFiManager::SIGNAL_GOOD:
            status_bar->setWifiIconState(3);
            break;
        case WiFiManager::SIGNAL_MODERATE:
            status_bar->setWifiIconState(2);
            break;
        case WiFiManager::SIGNAL_WEAK:
            status_bar->setWifiIconState(1);
            break;
        default:
            status_bar->setWifiIconState(0);
    }
    bsp_display_unlock();
}

/* 暂停应用（保留状态） */
bool AppSettings::pause(void)
{
    _is_ui_resumed = true; // 标记UI处于暂停状态
    return true;
}

/* 恢复应用 */
bool AppSettings::resume(void)
{
    _is_ui_resumed = false; // 清除暂停状态
    return true;
}

void AppSettings::extraUiInit(void)
{
    /* ----- 主界面设置 ----- */
    lv_label_set_text(ui_LabelPanelSettingMainContainer3Volume, "Audio");  // 设置音量标签文本
    lv_label_set_text(ui_LabelPanelSettingMainContainer4Light, "Display");  // 设置亮度标签文本
    
    // 对齐文本标签到对应图标的右侧
    lv_obj_align_to(ui_LabelPanelSettingMainContainer1WiFi, ui_ImagePanelSettingMainContainer1WiFi, LV_ALIGN_OUT_RIGHT_MID,
                    UI_MAIN_ITEM_LEFT_OFFSET, 0);  // WiFi文本右对齐图标
    lv_obj_align_to(ui_LabelPanelSettingMainContainer2Blue, ui_ImagePanelSettingMainContainer2Blue, LV_ALIGN_OUT_RIGHT_MID,
                    UI_MAIN_ITEM_LEFT_OFFSET, 0);  // 蓝牙文本右对齐图标
    lv_obj_align_to(ui_LabelPanelSettingMainContainer3Volume, ui_ImagePanelSettingMainContainer3Volume, LV_ALIGN_OUT_RIGHT_MID,
                    UI_MAIN_ITEM_LEFT_OFFSET, 0);  // 音量文本右对齐图标
    lv_obj_align_to(ui_LabelPanelSettingMainContainer4Light, ui_ImagePanelSettingMainContainer4Light, LV_ALIGN_OUT_RIGHT_MID,
                    UI_MAIN_ITEM_LEFT_OFFSET, 0);  // 亮度文本右对齐图标
    lv_obj_align_to(ui_LabelPanelSettingMainContainer5About, ui_ImagePanelSettingMainContainer5About, LV_ALIGN_OUT_RIGHT_MID,
                    UI_MAIN_ITEM_LEFT_OFFSET, 0);  // 关于文本右对齐图标

    // 记录屏幕索引并安装屏幕加载事件回调
    _screen_list[UI_MAIN_SETTING_INDEX] = ui_ScreenSettingMain;  // 存储主设置页对象
    lv_obj_add_event_cb(ui_ScreenSettingMain, onScreenLoadEventCallback, LV_EVENT_SCREEN_LOADED, this);  // 添加页面加载回调

    /* ----- WiFi设置界面 ----- */
    // 开关控件事件绑定
    lv_obj_add_event_cb(ui_SwitchPanelScreenSettingWiFiSwitch, onSwitchPanelScreenSettingWiFiSwitchValueChangeEventCallback,
                        LV_EVENT_VALUE_CHANGED, this);  // WiFi开关状态变更回调

    // 列表容器配置
    lv_obj_set_scroll_dir(ui_PanelScreenSettingWiFiList, LV_DIR_VER);  // 启用垂直滚动
    lv_obj_set_height(ui_PanelScreenSettingWiFiList, lv_pct(UI_WIFI_LIST_H_PERCENT));  // 设置高度百分比
    lv_obj_align_to(ui_PanelScreenSettingWiFiList, ui_PanelScreenSettingWiFiSwitch, LV_ALIGN_OUT_BOTTOM_MID, 0,
                    UI_WIFI_LIST_UP_OFFSET);  // 在开关下方定位列表
    lv_obj_set_style_pad_all(ui_PanelScreenSettingWiFiList, 0, 0);  // 清除默认内边距
    lv_obj_set_style_pad_top(ui_PanelScreenSettingWiFiList, UI_WIFI_LIST_UP_PAD, 0);  // 设置顶部内边距
    lv_obj_set_style_pad_bottom(ui_PanelScreenSettingWiFiList, UI_WIFI_LIST_DOWN_PAD, 0);  // 设置底部内边距

    // 动态创建WiFi列表项
    for(int i = 0; i < SCAN_LIST_SIZE; i++) {
        // 创建列表项容器
        panel_wifi_btn[i] = lv_obj_create(ui_PanelScreenSettingWiFiList);  // 创建按钮容器
        lv_obj_set_size(panel_wifi_btn[i], lv_pct(100), UI_WIFI_LIST_ITEM_H);  // 设置全宽和固定高度
        lv_obj_set_style_radius(panel_wifi_btn[i], 0, 0);  // 直角矩形
        lv_obj_set_style_border_width(panel_wifi_btn[i], 0, 0);  // 无边框
        lv_obj_set_style_text_font(panel_wifi_btn[i], UI_WIFI_LIST_ITEM_FONT, 0);  // 设置字体
        lv_obj_add_flag(panel_wifi_btn[i], LV_OBJ_FLAG_CLICKABLE);  // 启用点击交互
        lv_obj_clear_flag(panel_wifi_btn[i], LV_OBJ_FLAG_SCROLLABLE);  // 禁用独立滚动
        // 设置按压状态样式
        lv_obj_set_style_bg_color(panel_wifi_btn[i], lv_color_hex(0xCBCBCB), LV_PART_MAIN | LV_STATE_PRESSED);  // 按压背景色
        lv_obj_set_style_bg_opa(panel_wifi_btn[i], 255, LV_PART_MAIN| LV_STATE_DEFAULT);  // 默认不透明度
        lv_obj_set_style_border_color(panel_wifi_btn[i], lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);  // 边框色
        lv_obj_set_style_border_opa(panel_wifi_btn[i], 255, LV_PART_MAIN| LV_STATE_DEFAULT);  // 边框不透明度

        // 创建子元素
        label_wifi_ssid[i] = lv_label_create(panel_wifi_btn[i]);  // WiFi名称标签
        lv_obj_set_align(label_wifi_ssid[i], LV_ALIGN_LEFT_MID);  // 左中对齐

        img_img_wifi_lock[i] = lv_img_create(panel_wifi_btn[i]);  // 加密锁图标
        lv_obj_align(img_img_wifi_lock[i], LV_ALIGN_RIGHT_MID, UI_WIFI_ICON_LOCK_RIGHT_OFFSET, 0);  // 右侧定位
        lv_obj_add_flag(img_img_wifi_lock[i], LV_OBJ_FLAG_HIDDEN);  // 初始隐藏

        wifi_image[i] = lv_img_create(panel_wifi_btn[i]);  // 信号强度图标
        lv_obj_align(wifi_image[i], LV_ALIGN_RIGHT_MID, UI_WIFI_ICON_SIGNAL_RIGHT_OFFSET, 0);  // 右侧定位

        wifi_connect[i] = lv_label_create(panel_wifi_btn[i]);  // 连接状态标签
        lv_label_set_text(wifi_connect[i], LV_SYMBOL_OK);  // 显示√符号
        lv_obj_align(wifi_connect[i], LV_ALIGN_RIGHT_MID, UI_WIFI_ICON_CONNECT_RIGHT_OFFSET, 0);  // 右侧定位
        lv_obj_add_flag(wifi_connect[i], LV_OBJ_FLAG_HIDDEN);  // 初始隐藏

        // 绑定点击事件
        lv_obj_add_event_cb(panel_wifi_btn[i], onButtonWifiListClickedEventCallback, 
                           LV_EVENT_CLICKED, (void*)label_wifi_ssid[i]);  // 传递标签指针作为参数
        
        // 初始隐藏列表和加载动画（若未在扫描中）
        if(!(xEventGroupGetBits(s_wifi_event_group) & WIFI_EVENT_SCANING)) {
            lv_obj_add_flag(ui_PanelScreenSettingWiFiList, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(ui_SpinnerScreenSettingWiFi, LV_OBJ_FLAG_HIDDEN);
        }
    }
    lv_obj_add_flag(ui_ButtonScreenSettingWiFiReturn, LV_OBJ_FLAG_HIDDEN);  // 隐藏返回按钮

    /* ----- WiFi连接界面 ----- */
    lv_obj_add_flag(ui_SpinnerScreenSettingVerification, LV_OBJ_FLAG_HIDDEN);  // 隐藏验证页加载动画
    // 创建连接状态遮罩层
    _panel_wifi_connect = lv_obj_create(ui_ScreenSettingVerification);  // 创建半透明遮罩
    lv_obj_set_size(_panel_wifi_connect, lv_pct(100), lv_pct(100));  // 全屏尺寸
    lv_obj_set_style_bg_color(_panel_wifi_connect, lv_color_white(), 0);  // 白色背景
    lv_obj_set_style_bg_opa(_panel_wifi_connect, LV_OPA_50, 0);  // 50%透明度
    lv_obj_center(_panel_wifi_connect);  // 居中定位

    _img_wifi_connect = lv_img_create(_panel_wifi_connect);  // 连接结果图标
    lv_obj_center(_img_wifi_connect);  // 居中定位

    _spinner_wifi_connect = lv_spinner_create(_panel_wifi_connect, 1000, 600);  // 创建旋转加载指示器
    lv_obj_set_size(_spinner_wifi_connect, lv_pct(20), lv_pct(20));  // 设置尺寸百分比
    lv_obj_center(_spinner_wifi_connect);  // 居中定位
    processWifiConnect(WIFI_CONNECT_HIDE);  // 初始隐藏所有连接状态元素

    // 键盘配置
    lv_textarea_set_password_mode(ui_TextAreaScreenSettingVerificationPassword, true);  // 密码输入模式
    lv_obj_add_event_cb(ui_KeyboardScreenSettingVerification, onKeyboardScreenSettingVerificationClickedEventCallback,
                        LV_EVENT_CLICKED, this);  // 键盘按键回调

    // 记录屏幕索引并安装回调
    lv_obj_add_flag(ui_ButtonScreenSettingBLEReturn, LV_OBJ_FLAG_HIDDEN);  // 隐藏蓝牙返回按钮
    _screen_list[UI_WIFI_SCAN_INDEX] = ui_ScreenSettingWiFi;  // 存储WiFi扫描页对象
    lv_obj_add_event_cb(ui_ScreenSettingWiFi, onScreenLoadEventCallback, LV_EVENT_SCREEN_LOADED, this);  // 页面加载回调
    _screen_list[UI_WIFI_CONNECT_INDEX] = ui_ScreenSettingVerification;  // 存储WiFi连接页对象
    lv_obj_add_event_cb(ui_ScreenSettingVerification, onScreenLoadEventCallback, LV_EVENT_SCREEN_LOADED, this);  // 页面加载回调

    /* ----- 蓝牙设置界面 ----- */
    lv_obj_add_event_cb(ui_SwitchPanelScreenSettingBLESwitch, onSwitchPanelScreenSettingBLESwitchValueChangeEventCallback,
                        LV_EVENT_VALUE_CHANGED, this);  // 蓝牙开关状态变更回调
    _screen_list[UI_BLUETOOTH_SETTING_INDEX] = ui_ScreenSettingBLE;  // 存储蓝牙设置页对象
    lv_obj_add_event_cb(ui_ScreenSettingBLE, onScreenLoadEventCallback, LV_EVENT_SCREEN_LOADED, this);  // 页面加载回调

    /* ----- 显示设置界面 ----- */
    lv_slider_set_range(ui_SliderPanelScreenSettingLightSwitch1, SCREEN_BRIGHTNESS_MIN, SCREEN_BRIGHTNESS_MAX);  // 设置亮度滑块范围
    lv_obj_add_event_cb(ui_SliderPanelScreenSettingLightSwitch1, onSliderPanelLightSwitchValueChangeEventCallback,
                        LV_EVENT_VALUE_CHANGED, this);  // 亮度滑块变更回调
    lv_obj_add_flag(ui_ButtonScreenSettingLightReturn, LV_OBJ_FLAG_HIDDEN);  // 隐藏返回按钮
    _screen_list[UI_BRIGHTNESS_SETTING_INDEX] = ui_ScreenSettingLight;  // 存储亮度设置页对象
    lv_obj_add_event_cb(ui_ScreenSettingLight, onScreenLoadEventCallback, LV_EVENT_SCREEN_LOADED, this);  // 页面加载回调

    /* ----- 音频设置界面 ----- */
    lv_slider_set_range(ui_SliderPanelScreenSettingVolumeSwitch, SPEAKER_VOLUME_MIN, SPEAKER_VOLUME_MAX);  // 设置音量滑块范围
    lv_obj_add_event_cb(ui_SliderPanelScreenSettingVolumeSwitch, onSliderPanelVolumeSwitchValueChangeEventCallback,
                        LV_EVENT_VALUE_CHANGED, this);  // 音量滑块变更回调
    lv_obj_add_flag(ui_ButtonScreenSettingVolumeReturn, LV_OBJ_FLAG_HIDDEN);  // 隐藏返回按钮
    _screen_list[UI_VOLUME_SETTING_INDEX] = ui_ScreenSettingVolume;  // 存储音量设置页对象
    lv_obj_add_event_cb(ui_ScreenSettingVolume, onScreenLoadEventCallback, LV_EVENT_SCREEN_LOADED, this);  // 页面加载回调

    /* ----- 关于界面 ----- */
    lv_label_set_text(ui_LabelPanelPanelScreenSettingAbout4, "ESP_Brookesia");  // 设置框架名称
    lv_obj_add_flag(ui_ButtonScreenSettingAboutReturn, LV_OBJ_FLAG_HIDDEN);  // 隐藏返回按钮
    _screen_list[UI_ABOUT_SETTING_INDEX] = ui_ScreenSettingAbout;  // 存储关于页对象
    lv_obj_add_event_cb(ui_ScreenSettingAbout, onScreenLoadEventCallback, LV_EVENT_SCREEN_LOADED, this);  // 页面加载回调

    // 隐藏未使用的UI元素
    lv_obj_add_flag(ui_PanelSettingMainContainerItem2, LV_OBJ_FLAG_HIDDEN);  // 隐藏容器项2
    
    // 设置关于页面信息
    lv_label_set_text(ui_LabelPanelPanelScreenSettingAbout3, mac_str);  // 显示MAC地址
    lv_label_set_text(ui_LabelPanelPanelScreenSettingAbout5, "v0.2.0");  // 显示固件版本
    lv_label_set_text(ui_LabelPanelPanelScreenSettingAbout2, "Waveshare ESP32-P4 Board");  // 显示开发板型号
    lv_obj_set_x(ui_LabelPanelPanelScreenSettingAbout2, 167);  // 水平定位板型文本

    // 动态生成UI版本号
    char char_ui_version[20];
    snprintf(char_ui_version, sizeof(char_ui_version), "v%d.%d.%d", 
             ESP_BROOKESIA_CONF_VER_MAJOR,  // 主版本号
             ESP_BROOKESIA_CONF_VER_MINOR,  // 次版本号
             ESP_BROOKESIA_CONF_VER_PATCH); // 修订号
    lv_label_set_text(ui_LabelPanelPanelScreenSettingAbout6, char_ui_version);  // 显示UI版本
}
/* WiFi连接状态UI控制 */
void AppSettings::processWifiConnect(WifiConnectState_t state)
{
    switch (state) {
    case WIFI_CONNECT_HIDE:       // 隐藏所有元素
        lv_obj_add_flag(_panel_wifi_connect, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(_img_wifi_connect, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(_spinner_wifi_connect, LV_OBJ_FLAG_HIDDEN);
        break;
    case WIFI_CONNECT_RUNNING:    // 显示连接中状态
        lv_obj_clear_flag(_panel_wifi_connect, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(_img_wifi_connect, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(_spinner_wifi_connect, LV_OBJ_FLAG_HIDDEN);
        break;
    case WIFI_CONNECT_SUCCESS:    // 显示成功图标
        lv_obj_clear_flag(_panel_wifi_connect, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(_img_wifi_connect, LV_OBJ_FLAG_HIDDEN);
        lv_img_set_src(_img_wifi_connect, &img_wifi_connect_success);
        lv_obj_add_flag(_spinner_wifi_connect, LV_OBJ_FLAG_HIDDEN);
        break;
    case WIFI_CONNECT_FAIL:       // 显示失败图标
        lv_obj_clear_flag(_panel_wifi_connect, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(_img_wifi_connect, LV_OBJ_FLAG_HIDDEN);
        lv_img_set_src(_img_wifi_connect, &img_wifi_connect_fail);
        lv_obj_add_flag(_spinner_wifi_connect, LV_OBJ_FLAG_HIDDEN);
        break;
    }
}

/* NVS参数加载 */
bool AppSettings::loadNvsParam(void)
{
    esp_err_t err = ESP_OK;
    nvs_handle_t nvs_handle;
    // 打开NVS命名空间
    err = nvs_open(NVS_STORAGE_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "NVS打开失败: %s", esp_err_to_name(err));
        return false;
    }
    ESP_LOGI(TAG, "准备加载NVS");
    // 遍历所有预定义键
    for (auto& key_value : _nvs_param_map) {
        err = nvs_get_i32(nvs_handle, key_value.first.c_str(), &key_value.second);
        switch (err) {
        case ESP_OK:  // 成功读取
            ESP_LOGI(TAG, "加载 %s: %d", key_value.first.c_str(), key_value.second);
            break;
        case ESP_ERR_NVS_NOT_FOUND: // 未找到则初始化
            err = nvs_set_i32(nvs_handle, key_value.first.c_str(), key_value.second);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "设置%s失败: %s", key_value.first.c_str(), esp_err_to_name(err));
            }
            ESP_LOGW(TAG, "初始化 %s 默认值: %d", key_value.first.c_str(), key_value.second);
            break;
        default:  // 其他错误
            ESP_LOGE(TAG, "读取%s异常: %s", key_value.first.c_str(), esp_err_to_name(err));
            break;
        }
    }

    // 提交更改
    err = nvs_commit(nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "NVS提交失败: %s", esp_err_to_name(err));
        return false;
    }
    nvs_close(nvs_handle); // 关闭句柄
    return true;
}




bool AppSettings::setNvsParam(std::string key, int value)
{
    esp_err_t err = ESP_OK; // 定义错误状态变量，初始化为成功
    nvs_handle_t nvs_handle; // 声明NVS句柄，用于操作非易失性存储

    // 打开NVS命名空间（命名空间名为NVS_STORAGE_NAMESPACE），读写模式
    err = nvs_open(NVS_STORAGE_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) { // 检查打开是否失败
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err)); // 记录错误日志
        return false; // 返回失败
    }

    // 将整数value写入NVS，键名为key
    err = nvs_set_i32(nvs_handle, key.c_str(), value);
    if (err != ESP_OK) { // 检查写入是否失败
        ESP_LOGE(TAG, "Error (%s) setting %s", esp_err_to_name(err), key.c_str()); // 记录错误日志
    }

    

    // 提交更改，确保数据写入闪存
    err = nvs_commit(nvs_handle);
    if (err != ESP_OK) { // 检查提交是否失败
        ESP_LOGE(TAG, "Error (%s) committing NVS changes", esp_err_to_name(err)); // 记录错误日志
        return false; // 返回失败
    }
    
    nvs_close(nvs_handle); // 关闭NVS句柄，释放资源
    return true; // 返回成功
}






void AppSettings::updateUiByNvsParam(void)
{
    // 根据WiFi开关状态更新UI：若启用则添加"选中"状态，否则清除
    if (_nvs_param_map[NVS_KEY_WIFI_ENABLE]) {
        lv_obj_add_state(ui_SwitchPanelScreenSettingWiFiSwitch, LV_STATE_CHECKED);
    } else {
        lv_obj_clear_state(ui_SwitchPanelScreenSettingWiFiSwitch, LV_STATE_CHECKED);
    }

    // 根据蓝牙开关状态更新UI：若启用则添加"选中"状态，否则清除
    if (_nvs_param_map[NVS_KEY_BLE_ENABLE]) {
        lv_obj_add_state(ui_SwitchPanelScreenSettingBLESwitch, LV_STATE_CHECKED);
    } else {
        lv_obj_clear_state(ui_SwitchPanelScreenSettingBLESwitch, LV_STATE_CHECKED);
    }

    // 设置亮度滑块的值（无动画）
    lv_slider_set_value(ui_SliderPanelScreenSettingLightSwitch1, 
                        _nvs_param_map[NVS_KEY_DISPLAY_BRIGHTNESS], 
                        LV_ANIM_OFF);
    
    // 设置音量滑块的值（无动画）
    lv_slider_set_value(ui_SliderPanelScreenSettingVolumeSwitch, 
                        _nvs_param_map[NVS_KEY_AUDIO_VOLUME], 
                        LV_ANIM_OFF);
}


/* 启动WiFi扫描 */
void AppSettings::startWifiScan(void)
{
    // 使用WiFiManager的扫描功能
    WiFiManager::getInstance().startScan([this](const std::vector<wifi_ap_record_t>& aps) {
        if (_is_ui_del) return;
        
        bsp_display_lock(0);
        // 清空现有列表
        deinitWifiListButton();
        
        // 更新UI显示扫描结果
        for (int i = 0; i < min(aps.size(), (size_t)SCAN_LIST_SIZE); i++) {
            const wifi_ap_record_t& ap = aps[i];
            bool psk = (ap.authmode != WIFI_AUTH_OPEN);
            
            // 确定信号等级
            WiFiManager::SignalLevel level = WiFiManager::getInstance().getSignalLevel(ap.rssi);
            const lv_img_dsc_t* img_src = nullptr;
            switch(level) {
                case WiFiManager::SIGNAL_GOOD: img_src = &img_wifisignal_good; break;
                case WiFiManager::SIGNAL_MODERATE: img_src = &img_wifisignal_moderate; break;
                case WiFiManager::SIGNAL_WEAK: img_src = &img_wifisignal_wake; break;
                default: img_src = &img_wifisignal_absent;
            }
            
            // 更新UI元素
            lv_label_set_text(label_wifi_ssid[i], (const char*)ap.ssid);
            lv_img_set_src(wifi_image[i], img_src);
            
            if (psk) {
                lv_img_set_src(img_img_wifi_lock[i], &img_wifi_lock);
                lv_obj_clear_flag(img_img_wifi_lock[i], LV_OBJ_FLAG_HIDDEN);
            }
            
            // 检查是否当前连接的网络
            if (WiFiManager::getInstance().isConnectedTo((const char*)ap.ssid)) {
    lv_obj_clear_flag(wifi_connect[i], LV_OBJ_FLAG_HIDDEN);
} else {
    lv_obj_add_flag(wifi_connect[i], LV_OBJ_FLAG_HIDDEN);
}
          // lv_obj_set_hidden(wifi_connect[i], !WiFiManager::getInstance().isConnectedTo((const char*)ap.ssid));
        }
        
        // 显示列表，隐藏加载动画
        lv_obj_clear_flag(ui_PanelScreenSettingWiFiList, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_SpinnerScreenSettingWiFi, LV_OBJ_FLAG_HIDDEN);
        bsp_display_unlock();
    });
    
    // 更新UI状态
    bsp_display_lock(0);
    lv_obj_clear_flag(ui_SpinnerScreenSettingWiFi, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_SwitchPanelScreenSettingWiFiSwitch, LV_OBJ_FLAG_CLICKABLE);
    bsp_display_unlock();
}

/* 停止WiFi扫描 */
void AppSettings::stopWifiScan(void)
{

}

// 初始化单个WiFi列表项的UI元素
void AppSettings::initWifiListButton(
    lv_obj_t* lv_label_ssid,       // 显示WiFi名称的标签对象
    lv_obj_t* lv_img_wifi_lock,     // 显示加密锁图标的图像对象
    lv_obj_t* lv_wifi_img,          // 显示信号强度图标的图像对象
    lv_obj_t* lv_wifi_connect,     // 显示连接状态（√图标）的对象
    uint8_t* ssid,                 // 当前WiFi的SSID名称（字节数组）
    bool psk,                      // 标识是否需要密码（true=加密网络）
    WifiSignalStrengthLevel_t signal_strength  // 信号强度等级枚举值
)
{
    // 设置SSID标签文本：将字节数组格式化为字符串显示[10](@ref)
    lv_label_set_text_fmt(lv_label_ssid, "%s", (const char*)ssid);

    // 检查是否为当前连接的WiFi：比较扫描到的SSID与存储的连接SSID
     if (WiFiManager::getInstance().isConnectedTo((const char*)ssid)) {
        lv_obj_clear_flag(lv_wifi_connect, LV_OBJ_FLAG_HIDDEN);
    }

    // 处理加密网络
      if(psk) {
        lv_img_set_src(lv_img_wifi_lock, &img_wifi_lock);
        lv_obj_clear_flag(lv_img_wifi_lock, LV_OBJ_FLAG_HIDDEN);
    }

    // 根据信号强度设置对应图标[1,2](@ref)
    if (signal_strength == WIFI_SIGNAL_STRENGTH_GOOD) {
        lv_img_set_src(lv_wifi_img, &img_wifisignal_good);      // 强信号图标
    } else if (signal_strength == WIFI_SIGNAL_STRENGTH_MODERATE) {
        lv_img_set_src(lv_wifi_img, &img_wifisignal_moderate);  // 中等信号图标
    } else if (signal_strength == WIFI_SIGNAL_STRENGTH_WEAK) {
        lv_img_set_src(lv_wifi_img, &img_wifisignal_wake);      // 弱信号图标
    } else {
        lv_img_set_src(lv_wifi_img, &img_wifisignal_absent);   // 无信号图标
    }
}

// 重置所有WiFi列表项状态（隐藏辅助图标）
void AppSettings::deinitWifiListButton(void)
{
    // 遍历所有预定义的WiFi列表项
    for (int i = 0; i < SCAN_LIST_SIZE; i++) {
        // 隐藏加密锁图标
        lv_obj_add_flag(img_img_wifi_lock[i], LV_OBJ_FLAG_HIDDEN);
        // 隐藏连接状态图标（√）
        lv_obj_add_flag(wifi_connect[i], LV_OBJ_FLAG_HIDDEN);
    }
}






void AppSettings::onKeyboardScreenSettingVerificationClickedEventCallback(lv_event_t *e)
{
   AppSettings *app = (AppSettings *)lv_event_get_user_data(e);
    lv_obj_t *target = lv_event_get_target(e);

    // 绑定键盘到密码输入框
    lv_keyboard_set_textarea(target, ui_TextAreaScreenSettingVerificationPassword);

    // 处理连接按钮点击（ID=39）
    if(lv_keyboard_get_selected_btn(target) == 39) {
        app->processWifiConnect(WIFI_CONNECT_RUNNING);
        app->stopWifiScan();

        // 获取输入的SSID和密码
        const char* ssid = lv_label_get_text(ui_LabelScreenSettingVerificationSSID);
        const char* password = lv_textarea_get_text(ui_TextAreaScreenSettingVerificationPassword);
        
        // 使用WiFiManager进行连接（捕获 ssid 和 password）
        WiFiManager::getInstance().connect(ssid, password, [app, ssid, password](bool success) {
            bsp_display_lock(0);
            if (success) {
                app->processWifiConnect(WIFI_CONNECT_SUCCESS);
                app->saveWifiCredentialsToNvs(ssid, password);
            } else {
                app->processWifiConnect(WIFI_CONNECT_FAIL);
            }
            
            // 延迟后返回
            vTaskDelay(pdMS_TO_TICKS(WIFI_CONNECT_UI_WAIT_TIME_MS));
            app->processWifiConnect(WIFI_CONNECT_HIDE);
            lv_textarea_set_text(ui_TextAreaScreenSettingVerificationPassword, "");
            app->back();
            bsp_display_unlock();
        });
    }
}


void AppSettings::onScreenLoadEventCallback(lv_event_t *e)
{
    AppSettings *app = (AppSettings *)lv_event_get_user_data(e);  // 获取应用实例
    SettingScreenIndex_t last_scr_index = app->_screen_index;  // 记录上一个界面索引

    // 安全校验
    ESP_BROOKESIA_CHECK_NULL_GOTO(app, end, "Invalid app pointer");

    // 确定当前加载的屏幕索引
    for (int i = 0; i < UI_MAX_INDEX; i++) {
        if (app->_screen_list[i] == lv_event_get_target(e)) {
            app->_screen_index = (SettingScreenIndex_t)i;  // 更新当前索引
            break;
        }
    }

    // 离开扫描页时停止扫描
    if (last_scr_index == UI_WIFI_SCAN_INDEX) {
        app->stopWifiScan();
    }

    // 进入扫描页且WiFi启用时开始扫描
    if ((app->_screen_index == UI_WIFI_SCAN_INDEX) && 
        (app->_nvs_param_map[NVS_KEY_WIFI_ENABLE] == true)) {
        app->startWifiScan();
    }

end:
    return;  // 错误处理出口
}

void AppSettings::onSwitchPanelScreenSettingBLESwitchValueChangeEventCallback(lv_event_t *e) 
{
    lv_state_t state = lv_obj_get_state(ui_SwitchPanelScreenSettingBLESwitch);  // 获取开关状态
    AppSettings *app = (AppSettings *)lv_event_get_user_data(e);  // 获取应用实例

    // 安全校验
    ESP_BROOKESIA_CHECK_NULL_GOTO(app, end, "Invalid app pointer");

    if (state & LV_STATE_CHECKED) {  // 开关开启
        app->_nvs_param_map[NVS_KEY_BLE_ENABLE] = true;
        app->setNvsParam(NVS_KEY_BLE_ENABLE, 1);  // 保存到NVS
    } else {  // 开关关闭
        app->_nvs_param_map[NVS_KEY_BLE_ENABLE] = false;
        app->setNvsParam(NVS_KEY_BLE_ENABLE, 0);  // 保存到NVS
    }

end:
    return;  // 错误处理出口
}

void AppSettings::onSwitchPanelScreenSettingWiFiSwitchValueChangeEventCallback(lv_event_t *e) 
{
    lv_state_t state = lv_obj_get_state(ui_SwitchPanelScreenSettingWiFiSwitch);  // 获取开关状态
    AppSettings *app = (AppSettings *)lv_event_get_user_data(e);  // 获取应用实例

    // 安全校验
    ESP_BROOKESIA_CHECK_NULL_GOTO(app, end, "Invalid app pointer");

    if (state & LV_STATE_CHECKED) {  // 开关开启
        app->_nvs_param_map[NVS_KEY_WIFI_ENABLE] = true;
        app->setNvsParam(NVS_KEY_WIFI_ENABLE, 1);  // 保存到NVS
        // 若在扫描页则立即启动扫描
        if (app->_screen_index == UI_WIFI_SCAN_INDEX) {
            app->startWifiScan();
        }
    } else {  // 开关关闭
        app->_nvs_param_map[NVS_KEY_WIFI_ENABLE] = false;
        app->setNvsParam(NVS_KEY_WIFI_ENABLE, 0);  // 保存到NVS
        // 若在扫描页则停止扫描
        if (app->_screen_index == UI_WIFI_SCAN_INDEX) {
            app->stopWifiScan();
            // 断开现有连接
            if (xEventGroupGetBits(s_wifi_event_group) & WIFI_EVENT_CONNECTED) {
                ESP_ERROR_CHECK(esp_wifi_disconnect());
                app->status_bar->setWifiIconState(0);  // 更新状态栏图标
            }
        }
    }

end:
    return;  // 错误处理出口
}
void AppSettings::onButtonWifiListClickedEventCallback(lv_event_t *e)
{
    // 获取事件相关对象
    lv_obj_t *label_wifi_ssid = (lv_obj_t*)lv_event_get_user_data(e);  // 关联的标签对象
    lv_obj_t *btn = lv_event_get_target(e);  // 触发的按钮对象
    lv_area_t btn_click_area;  // 按钮点击区域
    lv_point_t point;  // 点击坐标

    // 校验点击是否在有效区域内
    lv_obj_get_click_area(btn, &btn_click_area);
    lv_indev_get_point(lv_indev_get_act(), &point);  // 获取输入设备坐标
    if (point.x < btn_click_area.x1 || point.x > btn_click_area.x2 ||
        point.y < btn_click_area.y1 || point.y > btn_click_area.y2) {
        return;  // 点击在区域外则忽略
    }

    // 跳转到密码验证界面
    lv_scr_load(ui_ScreenSettingVerification);
    // 设置界面显示的SSID
    lv_label_set_text_fmt(ui_LabelScreenSettingVerificationSSID, 
                         "%s", lv_label_get_text(label_wifi_ssid));

    // 停止后台扫描
    xEventGroupClearBits(s_wifi_event_group, WIFI_EVENT_SCANING);  // 清除扫描标志
    esp_wifi_scan_stop();  // 停止硬件扫描
}

void AppSettings::onSliderPanelVolumeSwitchValueChangeEventCallback(lv_event_t *e) 
{
    int volume = lv_slider_get_value(ui_SliderPanelScreenSettingVolumeSwitch);  // 获取滑块值
    AppSettings *app = (AppSettings *)lv_event_get_user_data(e);  // 获取应用实例

    // 安全校验
    ESP_BROOKESIA_CHECK_NULL_GOTO(app, end, "Invalid app pointer");

    // 仅当音量改变时处理
    if (volume != app->_nvs_param_map[NVS_KEY_AUDIO_VOLUME]) {
        // 设置硬件音量并校验
        if ((bsp_extra_codec_volume_set(volume, NULL) != ESP_OK) && 
            (bsp_extra_codec_volume_get() != volume)) {
            ESP_LOGE(TAG, "Set volume failed");
            // 恢复滑块原始值
            lv_slider_set_value(ui_SliderPanelScreenSettingVolumeSwitch, 
                               app->_nvs_param_map[NVS_KEY_AUDIO_VOLUME], 
                               LV_ANIM_OFF);
            return;
        }
        // 更新内存参数并保存到NVS
        app->_nvs_param_map[NVS_KEY_AUDIO_VOLUME] = volume;
        app->setNvsParam(NVS_KEY_AUDIO_VOLUME, volume);
    }

end:
    return;  // 错误处理出口
}

void AppSettings::onSliderPanelLightSwitchValueChangeEventCallback(lv_event_t *e) 
{
    brightness = lv_slider_get_value(ui_SliderPanelScreenSettingLightSwitch1);  // 获取滑块值
    AppSettings *app = (AppSettings *)lv_event_get_user_data(e);  // 获取应用实例

    // 安全校验
    ESP_BROOKESIA_CHECK_NULL_GOTO(app, end, "Invalid app pointer");

    // 仅当亮度改变时处理
    if (brightness != app->_nvs_param_map[NVS_KEY_DISPLAY_BRIGHTNESS]) {
        // 设置硬件亮度（简化版校验）
        if (bsp_display_brightness_set(brightness) != ESP_OK) {
            ESP_LOGE(TAG, "Set brightness failed");
            // 恢复滑块原始值
            lv_slider_set_value(ui_SliderPanelScreenSettingLightSwitch1, 
                               app->_nvs_param_map[NVS_KEY_DISPLAY_BRIGHTNESS], 
                               LV_ANIM_OFF);
            return;
        }
        // 更新内存参数并保存到NVS
        app->_nvs_param_map[NVS_KEY_DISPLAY_BRIGHTNESS] = brightness;
        app->setNvsParam(NVS_KEY_DISPLAY_BRIGHTNESS, brightness);
    }

end:
    return;  // 错误处理出口
}

/**
 * 将WIFI的SSID和密码保存到NVS
 * @param ssid WIFI SSID字符串
 * @param password WIFI密码字符串
 */
bool AppSettings::saveWifiCredentialsToNvs(const char *ssid, const char *password)
{
    if (!ssid || !password) {
        ESP_LOGE(TAG, "Invalid SSID or password");
        return false;
    }

    esp_err_t err = ESP_OK;
    nvs_handle_t nvs_handle;
    bool success = true;
    char *ssid_dup = strdup(ssid);
    char *pwd_dup = strdup(password);

    if (!ssid_dup || !pwd_dup) {
        ESP_LOGE(TAG, "Memory allocation failed");
        free(ssid_dup);
        free(pwd_dup);
        return false;
    }

    err = nvs_open(NVS_STORAGE_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        free(ssid_dup);
        free(pwd_dup);
        return false;
    }

    err = nvs_set_str(nvs_handle, NVS_KEY_WIFI_SSID, ssid_dup);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) setting SSID", esp_err_to_name(err));
        success = false;
    }

    err = nvs_set_str(nvs_handle, NVS_KEY_WIFI_PWD, pwd_dup);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) setting password", esp_err_to_name(err));
        success = false;
    }

    free(ssid_dup);
    free(pwd_dup);

    if (success) {
        err = nvs_commit(nvs_handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error (%s) committing NVS changes", esp_err_to_name(err));
            success = false;
        }
    }
   ESP_LOGI(TAG, "保存保存成功:%s   %s", ssid, password);
    nvs_close(nvs_handle);
    return success;
}


/**
 * 从NVS读取WIFI的SSID和密码
 * @param ssid 输出参数，用于存储读取的SSID
 * @param password 输出参数，用于存储读取的密码
 * @return 是否成功读取
 */
bool AppSettings::readWifiCredentialsFromNvs(char *ssid, char *password)
{
    esp_err_t err;
    nvs_handle_t nvs_handle;

    // 打开NVS命名空间
    err = nvs_open(NVS_STORAGE_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "NVS_WIFI打开失败: %s", esp_err_to_name(err));
        return false;
    }
    ESP_LOGI(TAG, "准备加载NVS_WIFI");

    // 读取SSID
    size_t required_size = 32; // 使用实际缓冲区大小
    err = nvs_get_str(nvs_handle, NVS_KEY_WIFI_SSID, ssid, &required_size);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "读取%s异常: %s", NVS_KEY_WIFI_SSID, esp_err_to_name(err));
        nvs_close(nvs_handle);
        return false;
    }
    ESP_LOGI(TAG, "加载 %s: %s", NVS_KEY_WIFI_SSID, ssid);

    // 读取密码
    required_size = 64; // 使用实际缓冲区大小
    err = nvs_get_str(nvs_handle, NVS_KEY_WIFI_PWD, password, &required_size);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "读取%s异常: %s", NVS_KEY_WIFI_PWD, esp_err_to_name(err));
        nvs_close(nvs_handle);
        return false;
    }
    ESP_LOGI(TAG, "加载 %s: %s", NVS_KEY_WIFI_PWD, password);

    nvs_close(nvs_handle);
    return true;
}

