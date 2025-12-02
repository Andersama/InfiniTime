#include "displayapp/screens/HeartRateZone.h"
#include <lvgl/lvgl.h>
#include <lvgl/lv_obj_style.h>
#include <components/heartrate/HeartRateController.h>

#include "displayapp/DisplayApp.h"
#include "displayapp/InfiniTimeTheme.h"

using namespace Pinetime::Applications::Screens;

namespace {
  const char* ToString(Pinetime::Controllers::HeartRateController::States s) {
    switch (s) {
      case Pinetime::Controllers::HeartRateController::States::NotEnoughData:
        return "Not enough data,\nplease wait...";
      case Pinetime::Controllers::HeartRateController::States::NoTouch:
        return "No touch detected";
      case Pinetime::Controllers::HeartRateController::States::Running:
        return "Measuring...";
      case Pinetime::Controllers::HeartRateController::States::Stopped:
        return "Stopped";
    }
    return "";
  }

  void btnStartStopEventHandler(lv_obj_t* obj, lv_event_t event) {
    auto* screen = static_cast<HeartRate*>(obj->user_data);
    screen->OnStartStopEvent(event);
  }
}

HeartRateZone::HeartRateZone(Controllers::HeartRateController& heartRateController, System::SystemTask& systemTask)
  : heartRateController {heartRateController}, wakeLock(systemTask) {
  bool isHrRunning = heartRateController.State() != Controllers::HeartRateController::States::Stopped;

  auto activity = heartRateController.Activity();
  uint32_t total = 0;

  auto hundreths_of_hour = pdMS_TO_TICKS(10*60*60);

  lv_obj_t* screen = lv_scr_act();
  for (uint8_t i = 0; i < zone_bar.size(); i++) {
    zone_bar[i] = lv_bar_create(screen, nullptr);
    
    //lv_bar_set_orientation(zone_bar[i], LV_BAR_ORIENTATION_HORIZONTAL);
    total += activity.zoneTime[i];
    lv_obj_set_style_local_line_color(zone_bar[i], LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, Colors::bgAlt);
    lv_obj_set_size(zone_bar[i], 240, 20);
    lv_obj_align(zone_bar[i], nullptr, LV_ALIGN_IN_TOP_LEFT, 10, 25 * i);

    label_time[i] = lv_label_create(zone_bar[i]);
    lv_obj_align(zone_bar[i], nullptr, LV_ALIGN_CENTER, 0, 0);
  }

  lv_label_set_text_static(label_time[0], "Warm Up");
  lv_label_set_text_static(label_time[1], "Recovery");
  lv_label_set_text_static(label_time[2], "Aerobic");
  lv_label_set_text_static(label_time[3], "Threshold");
  lv_label_set_text_static(label_time[4], "Anaerobic");

  lv_obj_set_style_local_line_color(zone_bar[0], LV_BAR_PART_INDIC, LV_STATE_DEFAULT, Colors::blue);
  lv_obj_set_style_local_line_color(zone_bar[1], LV_BAR_PART_INDIC, LV_STATE_DEFAULT, Colors::green);
  lv_obj_set_style_local_line_color(zone_bar[2], LV_BAR_PART_INDIC, LV_STATE_DEFAULT, Colors::orange);
  lv_obj_set_style_local_line_color(zone_bar[3], LV_BAR_PART_INDIC, LV_STATE_DEFAULT, Colors::deepOrange);
  lv_obj_set_style_local_line_color(zone_bar[4], LV_BAR_PART_INDIC, LV_STATE_DEFAULT, Colors::heartRed);

  auto bar_limit = total / hundreths_of_hour;

  for (uint8_t i = 0; i < zone_bar.size(); i++) {
    uint32_t percent = activity.zoneTime[i] / hundreths_of_hour;
    lv_bar_set_range(zone_bar[i], 0, bar_limit);
    lv_bar_set_value(zone_bar[i], percent, LV_ANIM_OFF);
  }

  taskRefresh = lv_task_create(RefreshTaskCallback, 5000, LV_TASK_PRIO_MID, this);
}

HeartRateZone::~HeartRateZone() {
  lv_task_del(taskRefresh);
  lv_obj_clean(lv_scr_act());
}

void HeartRateZone::Refresh() {
  auto activity = heartRateController.Activity();
  uint32_t total = 0;

  auto hundreths_of_hour = pdMS_TO_TICKS(10 * 60 * 60);

  for (uint8_t i = 0; i < zone_bar.size(); i++) {
    total += activity.zoneTime[i];
  }

  auto bar_limit = total / hundreths_of_hour;

  for (uint8_t i = 0; i < zone_bar.size(); i++) {
    uint32_t percent = activity.zoneTime[i] / hundreths_of_hour;
    lv_bar_set_range(zone_bar[i], 0, bar_limit);
    lv_bar_set_value(zone_bar[i], percent, LV_ANIM_OFF);
  }
}