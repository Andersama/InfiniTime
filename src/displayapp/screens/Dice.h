#pragma once

#include "displayapp/apps/Apps.h"
#include "displayapp/screens/Screen.h"
#include "displayapp/widgets/Counter.h"
#include "displayapp/Controllers.h"
#include "Symbols.h"

#include <array>
#include <cstdint>

namespace PCG {
  // *Really* minimal PCG32 code / (c) 2014 M.E. O'Neill / pcg-random.org
  // Licensed under Apache License 2.0 (NO WARRANTY, etc. see website)
  // Website: https://www.pcg-random.org/download.html
  // See: https://www.apache.org/licenses/GPL-compatibility.html
  typedef struct {
    uint64_t state;
    uint64_t inc;
  } pcg32_random_t;

  uint32_t pcg32_random_r(pcg32_random_t* rng);

  // Lemire's Method (slight rewrite) [0, range)
  uint32_t bounded_rand(pcg32_random_t& rng, uint32_t range);
};

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class Dice : public Screen {
      public:
        Dice(Controllers::MotionController& motionController,
             Controllers::MotorController& motorController,
             Controllers::Settings& settingsController);
        ~Dice() override;
        void Roll();
        void Refresh() override;

      private:
        lv_obj_t* btnRoll;
        lv_obj_t* btnRollLabel;
        lv_obj_t* resultTotalLabel;
        lv_obj_t* resultIndividualLabel;
        lv_task_t* refreshTask;
        bool enableShakeForDice = false;

        PCG::pcg32_random_t rng;

        std::array<lv_color_t, 3> resultColors = {LV_COLOR_YELLOW, LV_COLOR_MAGENTA, LV_COLOR_AQUA};
        uint8_t currentColorIndex;
        void NextColor();

        Widgets::Counter nCounter = Widgets::Counter(1, 9, jetbrains_mono_42);
        Widgets::Counter dCounter = Widgets::Counter(2, 99, jetbrains_mono_42);

        bool openingRoll = true;
        uint8_t currentRollHysteresis = 0;
        static constexpr uint8_t rollHysteresis = 10;

        Controllers::MotorController& motorController;
        Controllers::MotionController& motionController;
        Controllers::Settings& settingsController;
      };
    }

    template <>
    struct AppTraits<Apps::Dice> {
      static constexpr Apps app = Apps::Dice;
      static constexpr const char* icon = Screens::Symbols::dice;

      static Screens::Screen* Create(AppControllers& controllers) {
        return new Screens::Dice(controllers.motionController, controllers.motorController, controllers.settingsController);
      };

      static bool IsAvailable(Pinetime::Controllers::FS& /*filesystem*/) {
        return true;
      };
    };
  }
}
