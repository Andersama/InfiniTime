#pragma once
#include <cstdint>
#include <FreeRTOS.h>
#include <timers.h>
#include "components/motion/MotionController.h"

namespace Pinetime {
  namespace Controllers {
    struct RNG {
      using rng_uint = uint32_t;
      using rng_uint2 = uint64_t;
      using rng_out = uint16_t;

      struct pcg_random_t {
        rng_uint state = {};
        rng_uint inc = {};
      };

      using State = pcg_random_t;
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