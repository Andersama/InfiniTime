#include "components/rng/PCG.h"

Pinetime::Controllers::RNG::State Pinetime::Controllers::RNG::Seed(Pinetime::Controllers::RNG::rng_uint s,
                                                                   Pinetime::Controllers::RNG::rng_uint i) {
  rng.state = 0u;
  rng.inc = i | 1u;
  Generate();
  rng.state += s;
  Generate();
  return rng;
}

Pinetime::Controllers::RNG::State Pinetime::Controllers::RNG::Seed() {
  using namespace Pinetime::Controllers;
  Pinetime::Controllers::RNG::State new_rng;
  new_rng.state = (RNG::rng_uint) Generate() << (sizeof(new_rng.state) * 4) ^ (RNG::rng_uint) Generate();
  new_rng.inc = (RNG::rng_uint) Generate() << (sizeof(new_rng.inc) * 4) ^ (RNG::rng_uint) Generate();
  return new_rng;
}

// *Really* minimal PCG32 code / (c) 2014 M.E. O'Neill / pcg-random.org
// Licensed under Apache License 2.0 (NO WARRANTY, etc. see website)
// Website: https://www.pcg-random.org/download.html
// See: https://www.apache.org/licenses/GPL-compatibility.html
/*
    uint64_t oldstate = rng.state;
    // Advance internal state
    rng.state = oldstate * 6364136223846793005ULL + (rng.inc | 1);
    // Calculate output function (XSH RR), uses old state for max ILP
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
*/
Pinetime::Controllers::RNG::rng_out Pinetime::Controllers::RNG::Generate() {
  using namespace Pinetime::Controllers;
  // See magic numbers in https://github.com/imneme/pcg-cpp/blob/master/include/pcg_random.hpp
  constexpr uint32_t tbits = sizeof(rng.state) * 8;
  constexpr uint32_t sbits = sizeof(rng) * 8;
  constexpr uint32_t obits = sizeof(RNG::rng_out) * 8;

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
  // 64 bits of state, 32 output (64 - 5 = 59, 32 - 5 = 27 and floor((5+32)/2) = 18)
  // 2^5 = 32*
  // output = rotate<s3,s2,s1>(state ^ (state >> s1)) >> s2, state >> s3)
  // 32 bits of state, 16 output (32 - 4 = 28, 16 - 4 = 12, and floor((4 + 16)/2) = 10)
  // 2^4 = 16*
  constexpr uint32_t s3 = tbits - opbits;
  constexpr uint32_t s2 = obits - opbits;
  constexpr uint32_t s1 = (obits + 5) / 2;

  constexpr RNG::rng_out mask = (1 << opbits) - 1;
  RNG::rng_out xorshifted = ((oldstate >> s1) ^ oldstate) >> s2;
  rng_out rot = oldstate >> s3;
  // rotate
  return (xorshifted >> rot) | (xorshifted << ((-rot) & mask));
};

// Lemire's Method (slight rewrite) [0, range)
Pinetime::Controllers::RNG::rng_out Pinetime::Controllers::RNG::GenerateBounded(Pinetime::Controllers::RNG::rng_out range) {
  using namespace Pinetime::Controllers;
  rng_uint m;
  RNG::rng_out t = (-range) % range;
  RNG::rng_out l;

  do {
    RNG::rng_out x = Generate();
    m = RNG::rng_uint(x) * RNG::rng_uint(range);
    l = RNG::rng_out(m);
  } while (l < t);

  return m >> (sizeof(RNG::rng_out) * 8);
};