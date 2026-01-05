#include "monte_carlo_evaluator.h"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace tt_int {

MonteCarloEvaluator::MonteCarloEvaluator(size_t numSamples, std::optional<unsigned> seed)
    : numSamples_(numSamples) {
    if (seed.has_value()) {
        rng_.seed(seed.value());
    } else {
        std::random_device rd;
        rng_.seed(rd());
    }
}

SimulationResult MonteCarloEvaluator::evaluate(std::shared_ptr<Expression> expr,
                                               const VariableRegistry& registry) {
    SimulationResult result;
    result.samples.reserve(numSamples_);
    result.totalSampleCount = numSamples_;
    
    // Generate all samples
    for (size_t i = 0; i < numSamples_; ++i) {
        auto variables = registry.sampleAll(rng_);
        double value = expr->evaluate(variables);
        result.samples.push_back(value);
    }
    
    // Filter out NaN values for statistics
    std::vector<double> validSamples;
    validSamples.reserve(numSamples_);
    for (double sample : result.samples) {
        if (!std::isnan(sample)) {
            validSamples.push_back(sample);
        }
    }
    
    result.validSampleCount = validSamples.size();
    
    // Compute statistics on valid samples
    if (validSamples.empty()) {
        // All samples were NaN
        result.mean = std::numeric_limits<double>::quiet_NaN();
        result.stddev = std::numeric_limits<double>::quiet_NaN();
        result.min = std::numeric_limits<double>::quiet_NaN();
        result.max = std::numeric_limits<double>::quiet_NaN();
    } else {
        // Compute mean using standard algorithm
        result.mean = std::accumulate(validSamples.begin(), validSamples.end(), 0.0) 
                     / validSamples.size();
        
        // Compute standard deviation using Welford's algorithm for numerical stability
        double variance = 0.0;
        double mean = 0.0;
        size_t count = 0;
        
        for (double sample : validSamples) {
            count++;
            double delta = sample - mean;
            mean += delta / count;
            double delta2 = sample - mean;
            variance += delta * delta2;
        }
        
        if (count > 1) {
            variance /= (count - 1);  // Sample variance (unbiased estimator)
        } else {
            variance = 0.0;
        }
        
        result.stddev = std::sqrt(variance);
        
        // Compute min and max
        auto minmax = std::minmax_element(validSamples.begin(), validSamples.end());
        result.min = *minmax.first;
        result.max = *minmax.second;
    }
    
    return result;
}

} // namespace tt_int
