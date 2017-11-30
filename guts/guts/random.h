#ifndef GUTS_RANDOM_H
#define GUTS_RANDOM_H

#include <random>
#include <ctime>
#include <array>

namespace guts {

// Gets a seeded Mersenne Twister engine.
// This is needed because MinGW has a broken random_device, so we mix in the
// system time as well, as shown in https://stackoverflow.com/a/34493057
inline std::mt19937 GetMT_RNG() {
  std::random_device rand_dev;
  std::array<long unsigned int, 2> init = {
      rand_dev(),
      static_cast<long unsigned int>(time(nullptr)),
  };
  std::seed_seq sseq(init.begin(), init.end());
  std::mt19937 rng(sseq);
  return rng;
}

} // namespace guts

#endif //GUTS_RANDOM_H
