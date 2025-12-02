#pragma once

#include <cstdint>
#include <components/ble/HeartRateService.h>
#include "utility/CircularBuffer.h"
#include <array>

namespace Pinetime {
  namespace Applications {
    class HeartRateTask;
  }

  namespace System {
    class SystemTask;
  }

  namespace Controllers {
    template<typename T>
    struct HeartRateZones {
      // 1440 minutes in a day (11 bits), 86400 seconds (17 bits)
      T zoneTime[5]   = {};
      T totalTime() const {
        return zoneTime[0] + zoneTime[1] + zoneTime[2] + zoneTime[3] + zoneTime[4];
      }
    };

    constexpr uint8_t maxHeartRateEstimate(uint8_t age) {
      return 220 - age;
    };

    constexpr int16_t fixed_rounding(int16_t value, int16_t divisor) {
      // true evil: we use >>'s signed behavior to propagate the leading 1 across all bits, eg: we have 0xffff or 0x0000
      int16_t signed_value = value >> 15u; 
      int16_t half_divisor = (divisor / 2);
      return (value + half_divisor - (divisor & signed_value)) / divisor; // we replace the * by "1" with an &
    }

    template<size_t N>
    constexpr std::array<uint8_t, N> bpmZones(std::array<uint8_t, N>& percentages, uint8_t maxBeatsPerMinute) {
      std::array<uint8_t, N> targets {};
      const uint16_t bpm = maxBeatsPerMinute;
      for (uint32_t i = 0; i < N; i++) {
        targets[i++] = (uint8_t)fixed_rounding((uint16_t) percentages[i] * bpm, 100);
      }
      return targets;
    };

    struct HeartRateZoneSettings {
      uint8_t age = 25;
      uint8_t maxHeartRate = maxHeartRateEstimate(age);
      std::array<uint8_t, 5> percentTarget = {50, 60, 70, 80, 90};
      std::array<uint8_t, 5> bpmTarget = bpmZones(percentTarget,maxHeartRate);
      uint32_t adjustDelay = 300000;
    };

    HeartRateZoneSettings zoneSettings {};

    class HeartRateController {
    public:
      enum class States : uint8_t { Stopped, NotEnoughData, NoTouch, Running };

      HeartRateController() = default;
      void Enable();
      void Disable();
      void Update(States newState, uint8_t heartRate);

      void SetHeartRateTask(Applications::HeartRateTask* task);

      States State() const {
        return state;
      }

      uint8_t HeartRate() const {
        return heartRate;
      }

      void SetService(Pinetime::Controllers::HeartRateService* service);

    private:
      Applications::HeartRateTask* task = nullptr;
      States state = States::Stopped;
      uint8_t heartRate = 0;
      Pinetime::Controllers::HeartRateService* service = nullptr;

      uint32_t lastActiveTime = 0;
      // Heart Rate Zone Storage
      HeartRateZones<uint32_t> currentActivity = {};
      Utility::CircularBuffer<HeartRateZones<uint16_t>, 31> activity = {};
    };
  }
}