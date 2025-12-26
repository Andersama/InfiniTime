#include "components/rng/PCG.h"

Pinetime::Controllers::RNG::State Pinetime::Controllers::RNG::Seed(Pinetime::Controllers::RNG::rng_uint s,
                                                                   Pinetime::Controllers::RNG::rng_uint i) {
  return rng = State(s, i);
}

Pinetime::Controllers::RNG::State Pinetime::Controllers::RNG::Seed() {
  using namespace Pinetime::Controllers;
  Pinetime::Controllers::RNG::State new_rng((uint64_t) Generate() << 32 ^ (uint64_t) Generate(),
                                            (uint64_t) Generate() << 32 ^ (uint64_t) Generate());
  return new_rng;
}

Pinetime::Controllers::RNG::rng_out Pinetime::Controllers::RNG::Generate() {
  return rng();
};

// See pcg-cpp/sample/codebook.cpp
Pinetime::Controllers::RNG::rng_out Pinetime::Controllers::RNG::GenerateBounded(Pinetime::Controllers::RNG::rng_out range) {
  return rng(range);
};