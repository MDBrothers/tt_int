#include "distribution.h"

namespace tt_int {

// NormalDistribution implementation
NormalDistribution::NormalDistribution(double mean, double stddev)
    : mean_(mean), stddev_(stddev), dist_(mean, stddev) {}

double NormalDistribution::sample(std::mt19937& rng) const {
    return dist_(rng);
}

// UniformDistribution implementation
UniformDistribution::UniformDistribution(double min, double max)
    : min_(min), max_(max), dist_(min, max) {}

double UniformDistribution::sample(std::mt19937& rng) const {
    return dist_(rng);
}

} // namespace tt_int
