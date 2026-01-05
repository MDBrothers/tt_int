#ifndef VARIABLE_REGISTRY_H
#define VARIABLE_REGISTRY_H

#include <map>
#include <memory>
#include <random>
#include <string>
#include "distribution.h"

namespace tt_int {

/**
 * @brief Registry for managing variables with associated probability distributions
 * 
 * The VariableRegistry stores mappings from variable names to their distributions
 * and provides functionality to sample all variables at once.
 */
class VariableRegistry {
public:
    /**
     * @brief Register a variable with its distribution
     * @param name The name of the variable
     * @param dist The probability distribution for this variable
     * 
     * If a variable with this name already exists, it will be replaced.
     */
    void registerVariable(const std::string& name,
                         std::shared_ptr<Distribution> dist);
    
    /**
     * @brief Sample all registered variables once
     * @param rng Random number generator to use for sampling
     * @return Map of variable names to their sampled values
     */
    std::map<std::string, double> sampleAll(std::mt19937& rng) const;
    
    /**
     * @brief Check if a variable is registered
     * @param name The name of the variable to check
     * @return true if the variable exists, false otherwise
     */
    bool hasVariable(const std::string& name) const;
    
    /**
     * @brief Get the number of registered variables
     * @return The count of registered variables
     */
    size_t getVariableCount() const;
    
private:
    std::map<std::string, std::shared_ptr<Distribution>> variables_;
};

} // namespace tt_int

#endif // VARIABLE_REGISTRY_H
