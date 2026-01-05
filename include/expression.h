#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <map>
#include <memory>
#include <string>

namespace tt_int {

/**
 * @brief Abstract base class for expression tree nodes
 * 
 * All expression types (constants, variables, operations) derive from this class.
 * Expressions can be evaluated with a given set of variable values.
 */
class Expression {
public:
    virtual ~Expression() = default;
    
    /**
     * @brief Evaluate the expression with the given variable values
     * @param variables Map of variable names to their values
     * @return The result of evaluating the expression
     */
    virtual double evaluate(const std::map<std::string, double>& variables) const = 0;
};

/**
 * @brief Expression representing a constant numeric value
 */
class Constant : public Expression {
public:
    /**
     * @brief Construct a constant expression
     * @param value The constant value
     */
    explicit Constant(double value);
    
    double evaluate(const std::map<std::string, double>& variables) const override;
    
private:
    double value_;
};

/**
 * @brief Expression representing a variable that looks up its value at evaluation time
 */
class Variable : public Expression {
public:
    /**
     * @brief Construct a variable expression
     * @param name The name of the variable
     */
    explicit Variable(const std::string& name);
    
    double evaluate(const std::map<std::string, double>& variables) const override;
    
private:
    std::string name_;
};

/**
 * @brief Binary operators supported by BinaryOp
 */
enum class BinaryOperator {
    Add,
    Subtract,
    Multiply,
    Divide
};

/**
 * @brief Expression representing a binary operation on two sub-expressions
 */
class BinaryOp : public Expression {
public:
    /**
     * @brief Construct a binary operation expression
     * @param left Left operand expression
     * @param right Right operand expression
     * @param op The binary operator to apply
     */
    BinaryOp(std::shared_ptr<Expression> left,
             std::shared_ptr<Expression> right,
             BinaryOperator op);
    
    double evaluate(const std::map<std::string, double>& variables) const override;
    
private:
    std::shared_ptr<Expression> left_;
    std::shared_ptr<Expression> right_;
    BinaryOperator op_;
};

} // namespace tt_int

#endif // EXPRESSION_H
