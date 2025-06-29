// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.3
// LVGL version: 8.3.6
// Project name: UI_Setting

#include "../ui.h"

void ui_ScreenSettingVolume_screen_init(void)
{
    ui_ScreenSettingVolume = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(ui_ScreenSettingVolume, lv_color_hex(0xE5F3FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_ScreenSettingVolume, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_PanelScreenSettingVolumeSwitch = lv_obj_create(ui_ScreenSettingVolume);
    lv_obj_set_height(ui_PanelScreenSettingVolumeSwitch, 83);
    lv_obj_set_width(ui_PanelScreenSettingVolumeSwitch, lv_pct(90));
    lv_obj_set_x(ui_PanelScreenSettingVolumeSwitch, 43);
    lv_obj_set_y(ui_PanelScreenSettingVolumeSwitch, 77);
    lv_obj_clear_flag(ui_PanelScreenSettingVolumeSwitch, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    lv_obj_set_style_radius(ui_PanelScreenSettingVolumeSwitch, 20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_ImagePanelScreenSettingVolumeSwitch = lv_img_create(ui_PanelScreenSettingVolumeSwitch);
    lv_img_set_src(ui_ImagePanelScreenSettingVolumeSwitch, &ui_img_sound_png);
    lv_obj_set_width(ui_ImagePanelScreenSettingVolumeSwitch, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(ui_ImagePanelScreenSettingVolumeSwitch, LV_SIZE_CONTENT); /// 1
    lv_obj_set_y(ui_ImagePanelScreenSettingVolumeSwitch, 3);
    lv_obj_set_x(ui_ImagePanelScreenSettingVolumeSwitch, lv_pct(1));
    lv_obj_set_align(ui_ImagePanelScreenSettingVolumeSwitch, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_ImagePanelScreenSettingVolumeSwitch, LV_OBJ_FLAG_ADV_HITTEST);  /// Flags
    lv_obj_clear_flag(ui_ImagePanelScreenSettingVolumeSwitch, LV_OBJ_FLAG_SCROLLABLE); /// Flags

    ui_LabelPanelScreenSettingVolumeSwitch = lv_label_create(ui_PanelScreenSettingVolumeSwitch);
    lv_obj_set_width(ui_LabelPanelScreenSettingVolumeSwitch, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(ui_LabelPanelScreenSettingVolumeSwitch, LV_SIZE_CONTENT); /// 1
    lv_obj_set_y(ui_LabelPanelScreenSettingVolumeSwitch, 0);
    lv_obj_set_x(ui_LabelPanelScreenSettingVolumeSwitch, lv_pct(10));
    lv_obj_set_align(ui_LabelPanelScreenSettingVolumeSwitch, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_LabelPanelScreenSettingVolumeSwitch, "Volume");
    lv_obj_set_style_text_font(ui_LabelPanelScreenSettingVolumeSwitch, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_SliderPanelScreenSettingVolumeSwitch = lv_slider_create(ui_PanelScreenSettingVolumeSwitch);
    lv_obj_set_width(ui_SliderPanelScreenSettingVolumeSwitch, 270);
    lv_obj_set_height(ui_SliderPanelScreenSettingVolumeSwitch, 10);
    lv_obj_set_x(ui_SliderPanelScreenSettingVolumeSwitch, 162);
    lv_obj_set_y(ui_SliderPanelScreenSettingVolumeSwitch, 0);
    lv_obj_set_align(ui_SliderPanelScreenSettingVolumeSwitch, LV_ALIGN_CENTER);

    ui_PanelScreenSettingVolumeList = lv_obj_create(ui_ScreenSettingVolume);
    lv_obj_set_width(ui_PanelScreenSettingVolumeList, lv_pct(88));
    lv_obj_set_height(ui_PanelScreenSettingVolumeList, LV_SIZE_CONTENT); /// 1
    lv_obj_set_x(ui_PanelScreenSettingVolumeList, 25);
    lv_obj_set_y(ui_PanelScreenSettingVolumeList, 119);
    lv_obj_set_flex_flow(ui_PanelScreenSettingVolumeList, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_PanelScreenSettingVolumeList, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_add_flag(ui_PanelScreenSettingVolumeList, LV_OBJ_FLAG_HIDDEN); /// Flags
    lv_obj_set_style_radius(ui_PanelScreenSettingVolumeList, 10, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_SpinnerScreenSettingVolume = lv_spinner_create(ui_ScreenSettingVolume, 1000, 90);
    lv_obj_set_width(ui_SpinnerScreenSettingVolume, lv_pct(16));
    lv_obj_set_height(ui_SpinnerScreenSettingVolume, lv_pct(16));
    lv_obj_set_align(ui_SpinnerScreenSettingVolume, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_SpinnerScreenSettingVolume, LV_OBJ_FLAG_HIDDEN);      /// Flags
    lv_obj_clear_flag(ui_SpinnerScreenSettingVolume, LV_OBJ_FLAG_CLICKABLE); /// Flags
    lv_obj_set_style_arc_width(ui_SpinnerScreenSettingVolume, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_arc_color(ui_SpinnerScreenSettingVolume, lv_color_hex(0xC2C2C2), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui_SpinnerScreenSettingVolume, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui_SpinnerScreenSettingVolume, 5, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    ui_ButtonScreenSettingVolumeReturn = lv_btn_create(ui_ScreenSettingVolume);
    lv_obj_set_width(ui_ButtonScreenSettingVolumeReturn, 60);
    lv_obj_set_height(ui_ButtonScreenSettingVolumeReturn, 60);
    lv_obj_set_x(ui_ButtonScreenSettingVolumeReturn, -30);
    lv_obj_set_y(ui_ButtonScreenSettingVolumeReturn, 10);
    lv_obj_set_align(ui_ButtonScreenSettingVolumeReturn, LV_ALIGN_TOP_RIGHT);
    lv_obj_add_flag(ui_ButtonScreenSettingVolumeReturn, LV_OBJ_FLAG_SCROLL_ON_FOCUS); /// Flags
    lv_obj_clear_flag(ui_ButtonScreenSettingVolumeReturn, LV_OBJ_FLAG_SCROLLABLE);    /// Flags
    lv_obj_set_style_bg_color(ui_ButtonScreenSettingVolumeReturn, lv_color_hex(0xE5F3FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_ButtonScreenSettingVolumeReturn, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_ImageScreenSettingVolumeReturn = lv_img_create(ui_ButtonScreenSettingVolumeReturn);
    lv_img_set_src(ui_ImageScreenSettingVolumeReturn, &ui_img_return_png);
    lv_obj_set_width(ui_ImageScreenSettingVolumeReturn, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_height(ui_ImageScreenSettingVolumeReturn, LV_SIZE_CONTENT); /// 1
    lv_obj_set_align(ui_ImageScreenSettingVolumeReturn, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_ImageScreenSettingVolumeReturn, LV_OBJ_FLAG_ADV_HITTEST);  /// Flags
    lv_obj_clear_flag(ui_ImageScreenSettingVolumeReturn, LV_OBJ_FLAG_SCROLLABLE); /// Flags
    lv_obj_set_style_img_recolor(ui_ImageScreenSettingVolumeReturn, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(ui_ImageScreenSettingVolumeReturn, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(ui_ButtonScreenSettingVolumeReturn, ui_event_ButtonScreenSettingVolumeReturn, LV_EVENT_ALL, NULL);
}
