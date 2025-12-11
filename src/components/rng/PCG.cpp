#include "PCG.h"
#include "components/motion/MotionController.h"
#include "src/libs/lv_conf.h"
//#include "libs/mynewt-nimble/nimble/controller/include/controller/ble_ll.h"
//#include "libs/mynewt-nimble/nimble/controller/src/ble_ll_rand.c"
#include "components/ble/BleController.h"
#include "components/ble/NotificationManager.h"

namespace Pinetime {
  namespace Controllers {
    RNG::RNG(RNG::State pcg_state) : rng(pcg_state) {};

    RNG::RNG(Pinetime::Controllers::MotionController& motionController) {
      SeedController(motionController);
    };

    RNG::State RNG::SeedController(Pinetime::Controllers::MotionController& motionController) {
      //ble_ll_rand_data_get((uint8_t*) this, sizeof(RNG::State));
      rng.state ^=
        (static_cast<uint64_t>(xTaskGetTickCount()) << 32) ^ (static_cast<uint64_t>(motionController.NbSteps()) << 16) ^ (uint64_t) &rng;
      rng.inc ^= (static_cast<uint64_t>(motionController.X()) << 32) ^ (static_cast<uint64_t>(motionController.Y()) << 16) ^
                 static_cast<uint64_t>(motionController.Z());

      return rng;
    };

    RNG::State RNG::Seed() {
      RNG::State new_rng;
      new_rng.state = (uint64_t) Generate() << 32 ^ (uint64_t) Generate();
      new_rng.inc = (uint64_t) Generate() << 32 ^ (uint64_t) Generate();
      return new_rng;
    }

    RNG::rng_uint RNG::Generate() {
      // See magic numbers in https://github.com/imneme/pcg-cpp/blob/master/include/pcg_random.hpp
      /*
      uint64_t oldstate = rng.state;
      // Advance internal state
      rng.state = oldstate * 6364136223846793005ULL + (rng.inc | 1);
      // Calculate output function (XSH RR), uses old state for max ILP
      uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
      uint32_t rot = oldstate >> 59u;
      return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
      */
      constexpr uint32_t tbits = sizeof(rng.state) * 8;
      constexpr uint32_t sbits = sizeof(rng) * 8;

      constexpr rng_uint multiplier = tbits >= 64 ? 1442695040888963407ULL : tbits >= 32 ? 747796405U : tbits >= 16 ? 12829U : 141u;

      constexpr uint32_t opbits = sbits - 5 >= 64 ? 5u : // 128 bits -> 5
                                    sbits - 4 >= 32 ? 4u
                                                    : // 64  bits -> 4
                                    sbits - 3 >= 16 ? 3u
                                                    : // 32  bits -> 3
                                    sbits - 2 >= 8 ? 2u
                                                   : // 16  bits -> 2
                                    sbits - 1 >= 1 ? 1u
                                                   : // 8   bits -> 1
                                    0u;

      auto oldstate = rng.state;
      rng.state = oldstate * multiplier + (rng.inc | 1);

      constexpr rng_uint mask = (1u << opbits) - 1;
      constexpr rng_uint shift = opbits + (tbits + mask) / 2;

      rng_uint xorshifted = ((oldstate >> shift) ^ oldstate) >> (tbits - opbits); // 16-4
      rng_uint rot = oldstate >> (sbits - opbits);                                // 32-4
      return (xorshifted >> rot) | (xorshifted << ((-rot) & mask));
    };

    // Lemire's Method (slight rewrite) [0, range)
    RNG::rng_uint RNG::GenerateBounded(rng_uint range) {
      rng_uint2 m;
      rng_uint t = (-range) % range;
      rng_uint l;
      do {
        rng_uint x = Generate();
        m = rng_uint2(x) * rng_uint2(range);
        l = rng_uint(m);
      } while (l < t);
      return m >> 16;
    };
  }
}