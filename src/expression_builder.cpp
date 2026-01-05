#include "expression_builder.h"

namespace tt_int {

ExpressionBuilder::ExpressionBuilder(std::shared_ptr<Expression> expr)
    : expr_(expr) {}

ExpressionBuilder ExpressionBuilder::constant(double value) {
    return ExpressionBuilder(std::make_shared<Constant>(value));
}

ExpressionBuilder ExpressionBuilder::variable(const std::string& name) {
    return ExpressionBuilder(std::make_shared<Variable>(name));
}

ExpressionBuilder ExpressionBuilder::operator+(const ExpressionBuilder& other) const {
    return ExpressionBuilder(
        std::make_shared<BinaryOp>(expr_, other.expr_, BinaryOperator::Add)
    );
}

ExpressionBuilder ExpressionBuilder::operator-(const ExpressionBuilder& other) const {
    return ExpressionBuilder(
        std::make_shared<BinaryOp>(expr_, other.expr_, BinaryOperator::Subtract)
    );
}

ExpressionBuilder ExpressionBuilder::operator*(const ExpressionBuilder& other) const {
    return ExpressionBuilder(
        std::make_shared<BinaryOp>(expr_, other.expr_, BinaryOperator::Multiply)
    );
}

ExpressionBuilder ExpressionBuilder::operator/(const ExpressionBuilder& other) const {
    return ExpressionBuilder(
        std::make_shared<BinaryOp>(expr_, other.expr_, BinaryOperator::Divide)
    );
}

// Mixed operations: builder + double
ExpressionBuilder operator+(const ExpressionBuilder& lhs, double rhs) {
    return lhs + ExpressionBuilder::constant(rhs);
}

ExpressionBuilder operator-(const ExpressionBuilder& lhs, double rhs) {
    return lhs - ExpressionBuilder::constant(rhs);
}

ExpressionBuilder operator*(const ExpressionBuilder& lhs, double rhs) {
    return lhs * ExpressionBuilder::constant(rhs);
}

ExpressionBuilder operator/(const ExpressionBuilder& lhs, double rhs) {
    return lhs / ExpressionBuilder::constant(rhs);
}

// Mixed operations: double + builder
ExpressionBuilder operator+(double lhs, const ExpressionBuilder& rhs) {
    return ExpressionBuilder::constant(lhs) + rhs;
}

ExpressionBuilder operator-(double lhs, const ExpressionBuilder& rhs) {
    return ExpressionBuilder::constant(lhs) - rhs;
}

ExpressionBuilder operator*(double lhs, const ExpressionBuilder& rhs) {
    return ExpressionBuilder::constant(lhs) * rhs;
}

ExpressionBuilder operator/(double lhs, const ExpressionBuilder& rhs) {
    return ExpressionBuilder::constant(lhs) / rhs;
}

} // namespace tt_int
