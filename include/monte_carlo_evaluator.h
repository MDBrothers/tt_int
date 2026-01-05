#ifndef MONTE_CARLO_EVALUATOR_H
#define MONTE_CARLO_EVALUATOR_H

#include <vector>
#include <memory>
#include <random>
#include <optional>
#include "expression.h"
#include "variable_registry.h"

namespace tt_int {

/**
 * @brief Statistics at a specific point during simulation
 * 
 * Used to track convergence of statistics as more samples are collected.
 */
struct ConvergencePoint {
    size_t sampleCount;        ///< Number of samples at this point
    double mean;               ///< Running mean at this point
    double stddev;             ///< Running standard deviation at this point
    size_t validCount;         ///< Valid (non-NaN) samples at this point
};

/**
 * @brief Result of a Monte Carlo simulation
 * 
 * Contains the raw samples and computed statistics from the simulation.
 */
struct SimulationResult {
    std::vector<double> samples;        ///< All samples including NaN values
    double mean;                         ///< Mean of valid (non-NaN) samples
    double stddev;                       ///< Standard deviation of valid samples
    double min;                          ///< Minimum of valid samples
    double max;                          ///< Maximum of valid samples
    size_t validSampleCount;            ///< Number of non-NaN samples
    size_t totalSampleCount;            ///< Total number of samples
    std::vector<ConvergencePoint> convergenceHistory;  ///< Statistics at intervals
};

/**
 * @brief Monte Carlo evaluator for expressions with stochastic variables
 * 
 * Evaluates an expression multiple times, sampling variables from their
 * distributions each time, and computes statistical properties of the results.
 */
class MonteCarloEvaluator {
    size_t numSamples_;
    std::mt19937 rng_;
    
public:
    /**
     * @brief Construct a Monte Carlo evaluator
     * @param numSamples Number of samples to generate
     * @param seed Optional seed for reproducibility (uses random_device if not provided)
     */
    MonteCarloEvaluator(size_t numSamples, std::optional<unsigned> seed = std::nullopt);
    
    /**
     * @brief Evaluate an expression using Monte Carlo simulation
     * @param expr Expression to evaluate
     * @param registry Variable registry containing distributions
     * @param convergenceInterval Interval for recording convergence statistics
     *        - 0 (default): No convergence tracking
     *        - Positive N: Record every N samples
     *        - Negative: Use smart intervals (logarithmic/percentage-based)
     * @return Simulation results with statistics
     */
    SimulationResult evaluate(std::shared_ptr<Expression> expr,
                             const VariableRegistry& registry,
                             int convergenceInterval = 0);
    
private:
    /**
     * @brief Compute smart convergence intervals based on total samples
     * @param totalSamples Total number of samples in simulation
     * @return Vector of sample counts at which to record statistics
     */
    std::vector<size_t> computeSmartIntervals(size_t totalSamples) const;
};

} // namespace tt_int

#endif // MONTE_CARLO_EVALUATOR_H
