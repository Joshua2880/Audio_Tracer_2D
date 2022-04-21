#include "Random.h"

std::random_device Random::dev_{};
std::mt19937 Random::rng_{ dev_() };
std::uniform_real_distribution<double> Random::dist_{ 0, 1 };