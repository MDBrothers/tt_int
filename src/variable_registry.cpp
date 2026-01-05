#include "variable_registry.h"

namespace tt_int {

void VariableRegistry::registerVariable(const std::string& name,
                                       std::shared_ptr<Distribution> dist) {
    variables_[name] = dist;
}

std::map<std::string, double> VariableRegistry::sampleAll(std::mt19937& rng) const {
    std::map<std::string, double> samples;
    for (const auto& pair : variables_) {
        samples[pair.first] = pair.second->sample(rng);
    }
    return samples;
}

bool VariableRegistry::hasVariable(const std::string& name) const {
    return variables_.find(name) != variables_.end();
}

size_t VariableRegistry::getVariableCount() const {
    return variables_.size();
}

} // namespace tt_int
