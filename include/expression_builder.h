#ifndef EXPRESSION_BUILDER_H
#define EXPRESSION_BUILDER_H

#include "expression.h"
#include <memory>
#include <string>

namespace tt_int {

/**
 * @brief Fluent API for building expression trees with natural C++ syntax
 * 
 * Provides operator overloading for arithmetic operations, allowing
 * expressions to be constructed using familiar mathematical notation.
 */
class ExpressionBuilder {
private:
    std::shared_ptr<Expression> expr_;

public:
    /**
     * @brief Construct builder from existing expression
     * @param expr The expression to wrap
     */
    explicit ExpressionBuilder(std::shared_ptr<Expression> expr);

    /**
     * @brief Create a constant expression
     * @param value The constant value
     * @return ExpressionBuilder wrapping a Constant expression
     */
    static ExpressionBuilder constant(double value);

    /**
     * @brief Create a variable expression
     * @param name The variable name
     * @return ExpressionBuilder wrapping a Variable expression
     */
    static ExpressionBuilder variable(const std::string& name);

    /**
     * @brief Addition operator
     * @param other The right-hand operand
     * @return New ExpressionBuilder representing this + other
     */
    ExpressionBuilder operator+(const ExpressionBuilder& other) const;

    /**
     * @brief Subtraction operator
     * @param other The right-hand operand
     * @return New ExpressionBuilder representing this - other
     */
    ExpressionBuilder operator-(const ExpressionBuilder& other) const;

    /**
     * @brief Multiplication operator
     * @param other The right-hand operand
     * @return New ExpressionBuilder representing this * other
     */
    ExpressionBuilder operator*(const ExpressionBuilder& other) const;

    /**
     * @brief Division operator
     * @param other The right-hand operand
     * @return New ExpressionBuilder representing this / other
     */
    ExpressionBuilder operator/(const ExpressionBuilder& other) const;

    /**
     * @brief Get the underlying expression
     * @return Shared pointer to the wrapped expression
     */
    std::shared_ptr<Expression> get() const { return expr_; }
};

// Mixed operations: builder + double
/**
 * @brief Addition with constant on right
 * @param lhs The builder operand
 * @param rhs The constant value
 * @return New ExpressionBuilder representing lhs + rhs
 */
ExpressionBuilder operator+(const ExpressionBuilder& lhs, double rhs);

/**
 * @brief Subtraction with constant on right
 * @param lhs The builder operand
 * @param rhs The constant value
 * @return New ExpressionBuilder representing lhs - rhs
 */
ExpressionBuilder operator-(const ExpressionBuilder& lhs, double rhs);

/**
 * @brief Multiplication with constant on right
 * @param lhs The builder operand
 * @param rhs The constant value
 * @return New ExpressionBuilder representing lhs * rhs
 */
ExpressionBuilder operator*(const ExpressionBuilder& lhs, double rhs);

/**
 * @brief Division with constant on right
 * @param lhs The builder operand
 * @param rhs The constant value
 * @return New ExpressionBuilder representing lhs / rhs
 */
ExpressionBuilder operator/(const ExpressionBuilder& lhs, double rhs);

// Mixed operations: double + builder
/**
 * @brief Addition with constant on left
 * @param lhs The constant value
 * @param rhs The builder operand
 * @return New ExpressionBuilder representing lhs + rhs
 */
ExpressionBuilder operator+(double lhs, const ExpressionBuilder& rhs);

/**
 * @brief Subtraction with constant on left
 * @param lhs The constant value
 * @param rhs The builder operand
 * @return New ExpressionBuilder representing lhs - rhs
 */
ExpressionBuilder operator-(double lhs, const ExpressionBuilder& rhs);

/**
 * @brief Multiplication with constant on left
 * @param lhs The constant value
 * @param rhs The builder operand
 * @return New ExpressionBuilder representing lhs * rhs
 */
ExpressionBuilder operator*(double lhs, const ExpressionBuilder& rhs);

/**
 * @brief Division with constant on left
 * @param lhs The constant value
 * @param rhs The builder operand
 * @return New ExpressionBuilder representing lhs / rhs
 */
ExpressionBuilder operator/(double lhs, const ExpressionBuilder& rhs);

} // namespace tt_int

#endif // EXPRESSION_BUILDER_H
