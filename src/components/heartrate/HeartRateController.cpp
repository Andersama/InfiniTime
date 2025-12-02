#include "components/heartrate/HeartRateController.h"
#include <heartratetask/HeartRateTask.h>
#include <systemtask/SystemTask.h>

using namespace Pinetime::Controllers;

void HeartRateController::Update(HeartRateController::States newState, uint8_t heartRate) {
  this->state = newState;
  if (this->heartRate != heartRate) {

    uint32_t ts = xTaskGetTickCount();
    uint32_t zone;
    //auto adjustMax = pdMS_TO_TICKS(300000); // 5 minutes
    for (zone = zoneSettings.bpmTarget.size() - 1; i < zoneSettings.bpmTarget.size(); --i) {
      if (this->heartRate >= zoneSettings.bpmTarget[i]) {
        uint32_t dt = ts - lastActiveTime;
        currentActivity.zoneTime[i] += dt;

        // don't make increases unless this is consistantly higher than normal (zone 5 is max)
        if (zone >= 4 && dt > zoneSettings.adjustDelay) {
          zoneSettings.maxHeartRate = zoneSettings.maxHeartRate >= this->heartRate ? zoneSettings.maxHeartRate : this->heartRate;   
        }
        break;
      }
    }
    lastActiveTime = ts;

    this->heartRate = heartRate;

    service->OnNewHeartRateValue(heartRate);
  }
}

void HeartRateController::Enable() {
  if (task != nullptr) {
    state = States::NotEnoughData;
    task->PushMessage(Pinetime::Applications::HeartRateTask::Messages::Enable);
  }
}

void HeartRateController::Disable() {
  if (task != nullptr) {
    state = States::Stopped;
    task->PushMessage(Pinetime::Applications::HeartRateTask::Messages::Disable);
  }
}

void HeartRateController::SetHeartRateTask(Pinetime::Applications::HeartRateTask* task) {
  this->task = task;
}

void HeartRateController::SetService(Pinetime::Controllers::HeartRateService* service) {
  this->service = service;
}
