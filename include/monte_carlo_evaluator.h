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
     * @return Simulation results with statistics
     */
    SimulationResult evaluate(std::shared_ptr<Expression> expr,
                             const VariableRegistry& registry);
};

} // namespace tt_int

#endif // MONTE_CARLO_EVALUATOR_H
