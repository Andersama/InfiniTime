#pragma once
#include <cstdint>
#include "components/motion/MotionController.h"

namespace PCG {
  // *Really* minimal PCG32 code / (c) 2014 M.E. O'Neill / pcg-random.org
  // Licensed under Apache License 2.0 (NO WARRANTY, etc. see website)
  // Website: https://www.pcg-random.org/download.html
  // See: https://www.apache.org/licenses/GPL-compatibility.html
  template<typename T>
  struct pcg_random_t {
    T state;
    T inc;
  };
};

namespace Pinetime {
  namespace Controllers {
    class RNG {
      using rng_uint  = uint16_t;
      using rng_uint2 = uint32_t;
      using State = PCG::pcg_random_t<rng_uint>;
      
      State rng;

    public:
      // Seed this RNG struct using data from motionController and time*
      State SeedController(Pinetime::Controllers::MotionController& motionController);
      // Generate another RNG struct with data generated via this one
      State Seed();

      // Produces an unsigned result within the full range of the data type
      rng_uint Generate();
      // Produces an unsigned result within [0, range)
      rng_uint GenerateBounded(rng_uint range);

      RNG() = default;
      RNG(State pcg_state);
      RNG(Pinetime::Controllers::MotionController& motionController);
      ~RNG() = default;
    };
  }
}