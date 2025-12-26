#pragma once
#include <cstdint>
#include <FreeRTOS.h>
#include <timers.h>
#include "components/motion/MotionController.h"
#include "pcg-cpp/include/pcg_random.hpp"

namespace Pinetime {
  namespace Controllers {
    struct RNG {

      /*
      struct pcg_random_t {
        rng_uint state = {};
        rng_uint inc = {};
      };
      */
      using State = pcg32;
      using rng_uint = State::state_type;// uint32_t;
      using rng_out = State::result_type;// uint16_t;

      State rng = {};

      State Seed(rng_uint s, rng_uint i);
      // Generate another RNG struct with data generated via this one
      State Seed();
      // Produces an unsigned result within the full range of the data type
      rng_out Generate();
      // Produces an unsigned result within [0, range)
      rng_out GenerateBounded(rng_out range);

      RNG() : rng() {};

      RNG& operator=(const State& pcg_state) {
        rng = pcg_state;
        return *this;
      };

      RNG(State pcg_state) {
        rng = pcg_state;
      };

      ~RNG() = default;
    };
  }
}