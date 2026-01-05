#include "expression.h"
#include <cmath>
#include <limits>
#include <stdexcept>

namespace tt_int {

// Constant implementation
Constant::Constant(double value) : value_(value) {}

double Constant::evaluate(const std::map<std::string, double>& variables) const {
    (void)variables; // Unused parameter
    return value_;
}

// Variable implementation
Variable::Variable(const std::string& name) : name_(name) {}

double Variable::evaluate(const std::map<std::string, double>& variables) const {
    auto it = variables.find(name_);
    if (it == variables.end()) {
        throw std::out_of_range("Variable '" + name_ + "' not found in variable map");
    }
    return it->second;
}

// BinaryOp implementation
BinaryOp::BinaryOp(std::shared_ptr<Expression> left,
                   std::shared_ptr<Expression> right,
                   BinaryOperator op)
    : left_(left), right_(right), op_(op) {}

double BinaryOp::evaluate(const std::map<std::string, double>& variables) const {
    double leftValue = left_->evaluate(variables);
    double rightValue = right_->evaluate(variables);
    
    switch (op_) {
        case BinaryOperator::Add:
            return leftValue + rightValue;
        
        case BinaryOperator::Subtract:
            return leftValue - rightValue;
        
        case BinaryOperator::Multiply:
            return leftValue * rightValue;
        
        case BinaryOperator::Divide:
            // Check for division by zero
            if (rightValue == 0.0) {
                return std::numeric_limits<double>::quiet_NaN();
            }
            return leftValue / rightValue;
        
        default:
            throw std::logic_error("Unknown binary operator");
    }
}

} // namespace tt_int
