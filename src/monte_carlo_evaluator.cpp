#include "monte_carlo_evaluator.h"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <set>

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

std::vector<size_t> MonteCarloEvaluator::computeSmartIntervals(size_t totalSamples) const {
    std::set<size_t> intervals;
    
    // Logarithmic intervals: powers of 10
    for (size_t power = 10; power < totalSamples; power *= 10) {
        intervals.insert(power);
    }
    
    // Percentage milestones
    std::vector<double> percentages = {0.01, 0.05, 0.10, 0.25, 0.50, 0.75, 0.90, 0.95, 1.00};
    for (double pct : percentages) {
        size_t count = static_cast<size_t>(pct * totalSamples);
        if (count > 0 && count <= totalSamples) {
            intervals.insert(count);
        }
    }
    
    // Always include the final count
    intervals.insert(totalSamples);
    
    return std::vector<size_t>(intervals.begin(), intervals.end());
}

SimulationResult MonteCarloEvaluator::evaluate(std::shared_ptr<Expression> expr,
                                               const VariableRegistry& registry,
                                               int convergenceInterval) {
    SimulationResult result;
    result.samples.reserve(numSamples_);
    result.totalSampleCount = numSamples_;
    
    // Determine which sample counts to record
    std::vector<size_t> recordPoints;
    if (convergenceInterval > 0) {
        // Fixed interval
        for (size_t i = static_cast<size_t>(convergenceInterval); i <= numSamples_; 
             i += static_cast<size_t>(convergenceInterval)) {
            recordPoints.push_back(i);
        }
        if (recordPoints.empty() || recordPoints.back() != numSamples_) {
            recordPoints.push_back(numSamples_);
        }
    } else if (convergenceInterval < 0) {
        // Smart intervals
        recordPoints = computeSmartIntervals(numSamples_);
    }
    // If convergenceInterval == 0, recordPoints remains empty (no tracking)
    
    // Welford's algorithm variables for online statistics
    double runningMean = 0.0;
    double runningM2 = 0.0;  // Sum of squared differences from mean
    size_t validCount = 0;
    size_t nextRecordIndex = 0;
    
    // Generate all samples
    for (size_t i = 0; i < numSamples_; ++i) {
        auto variables = registry.sampleAll(rng_);
        double value = expr->evaluate(variables);
        result.samples.push_back(value);
        
        // Update running statistics if value is valid
        if (!std::isnan(value)) {
            validCount++;
            double delta = value - runningMean;
            runningMean += delta / validCount;
            double delta2 = value - runningMean;
            runningM2 += delta * delta2;
        }
        
        // Check if we should record at this point
        size_t currentSampleCount = i + 1;
        if (nextRecordIndex < recordPoints.size() && 
            currentSampleCount == recordPoints[nextRecordIndex]) {
            
            ConvergencePoint point;
            point.sampleCount = currentSampleCount;
            point.validCount = validCount;
            
            if (validCount > 0) {
                point.mean = runningMean;
                point.stddev = validCount > 1 ? std::sqrt(runningM2 / (validCount - 1)) : 0.0;
            } else {
                point.mean = std::numeric_limits<double>::quiet_NaN();
                point.stddev = std::numeric_limits<double>::quiet_NaN();
            }
            
            result.convergenceHistory.push_back(point);
            nextRecordIndex++;
        }
    }
    
    result.validSampleCount = validCount;
    
    // Compute final statistics
    if (validCount == 0) {
        // All samples were NaN
        result.mean = std::numeric_limits<double>::quiet_NaN();
        result.stddev = std::numeric_limits<double>::quiet_NaN();
        result.min = std::numeric_limits<double>::quiet_NaN();
        result.max = std::numeric_limits<double>::quiet_NaN();
    } else {
        result.mean = runningMean;
        result.stddev = validCount > 1 ? std::sqrt(runningM2 / (validCount - 1)) : 0.0;
        
        // Compute min and max from valid samples
        std::vector<double> validSamples;
        validSamples.reserve(validCount);
        for (double sample : result.samples) {
            if (!std::isnan(sample)) {
                validSamples.push_back(sample);
            }
        }
        
        auto minmax = std::minmax_element(validSamples.begin(), validSamples.end());
        result.min = *minmax.first;
        result.max = *minmax.second;
    }
    
    return result;
}

} // namespace tt_int
