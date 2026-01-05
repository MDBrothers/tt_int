#include <gtest/gtest.h>
#include <cmath>
#include "expression.h"

using namespace tt_int;

// Test Constant evaluation
TEST(ExpressionTest, ConstantEvaluation) {
    auto expr = std::make_shared<Constant>(42.0);
    EXPECT_EQ(expr->evaluate({}), 42.0);
}

TEST(ExpressionTest, ConstantNegative) {
    auto expr = std::make_shared<Constant>(-3.14);
    EXPECT_EQ(expr->evaluate({}), -3.14);
}

// Test Variable lookup
TEST(ExpressionTest, VariableLookup) {
    auto expr = std::make_shared<Variable>("x");
    std::map<std::string, double> vars = {{"x", 5.0}};
    EXPECT_EQ(expr->evaluate(vars), 5.0);
}

TEST(ExpressionTest, VariableLookupMultiple) {
    auto expr = std::make_shared<Variable>("y");
    std::map<std::string, double> vars = {{"x", 1.0}, {"y", 2.0}, {"z", 3.0}};
    EXPECT_EQ(expr->evaluate(vars), 2.0);
}

TEST(ExpressionTest, VariableMissing) {
    auto expr = std::make_shared<Variable>("missing");
    std::map<std::string, double> vars = {{"x", 1.0}};
    EXPECT_THROW(expr->evaluate(vars), std::out_of_range);
}

// Test Addition
TEST(ExpressionTest, AddOperation) {
    auto expr = std::make_shared<BinaryOp>(
        std::make_shared<Constant>(5.0),
        std::make_shared<Constant>(3.0),
        BinaryOperator::Add
    );
    EXPECT_EQ(expr->evaluate({}), 8.0);
}

// Test Subtraction
TEST(ExpressionTest, SubtractOperation) {
    auto expr = std::make_shared<BinaryOp>(
        std::make_shared<Constant>(5.0),
        std::make_shared<Constant>(3.0),
        BinaryOperator::Subtract
    );
    EXPECT_EQ(expr->evaluate({}), 2.0);
}

// Test Multiplication
TEST(ExpressionTest, MultiplyOperation) {
    auto expr = std::make_shared<BinaryOp>(
        std::make_shared<Constant>(5.0),
        std::make_shared<Constant>(3.0),
        BinaryOperator::Multiply
    );
    EXPECT_EQ(expr->evaluate({}), 15.0);
}

// Test Division
TEST(ExpressionTest, DivideOperation) {
    auto expr = std::make_shared<BinaryOp>(
        std::make_shared<Constant>(6.0),
        std::make_shared<Constant>(3.0),
        BinaryOperator::Divide
    );
    EXPECT_EQ(expr->evaluate({}), 2.0);
}

// Test Divide by zero
TEST(ExpressionTest, DivideByZero) {
    auto expr = std::make_shared<BinaryOp>(
        std::make_shared<Constant>(5.0),
        std::make_shared<Constant>(0.0),
        BinaryOperator::Divide
    );
    double result = expr->evaluate({});
    EXPECT_TRUE(std::isnan(result));
}

// Test nested expressions
TEST(ExpressionTest, NestedExpression) {
    // (5 + 3) * 2 = 16
    auto addExpr = std::make_shared<BinaryOp>(
        std::make_shared<Constant>(5.0),
        std::make_shared<Constant>(3.0),
        BinaryOperator::Add
    );
    auto mulExpr = std::make_shared<BinaryOp>(
        addExpr,
        std::make_shared<Constant>(2.0),
        BinaryOperator::Multiply
    );
    EXPECT_EQ(mulExpr->evaluate({}), 16.0);
}

TEST(ExpressionTest, NestedWithVariables) {
    // (x + y) * 2 with x=10, y=20 = 60
    std::map<std::string, double> vars = {{"x", 10.0}, {"y", 20.0}};
    
    auto addExpr = std::make_shared<BinaryOp>(
        std::make_shared<Variable>("x"),
        std::make_shared<Variable>("y"),
        BinaryOperator::Add
    );
    auto mulExpr = std::make_shared<BinaryOp>(
        addExpr,
        std::make_shared<Constant>(2.0),
        BinaryOperator::Multiply
    );
    EXPECT_EQ(mulExpr->evaluate(vars), 60.0);
}

TEST(ExpressionTest, ComplexNested) {
    // ((x * y) + (a - b)) / (c + d)
    // x=4, y=5, a=10, b=3, c=2, d=5
    // ((4*5) + (10-3)) / (2+5) = (20 + 7) / 7 = 27/7 ≈ 3.857
    std::map<std::string, double> vars = {
        {"x", 4.0}, {"y", 5.0}, {"a", 10.0}, 
        {"b", 3.0}, {"c", 2.0}, {"d", 5.0}
    };
    
    auto mulExpr = std::make_shared<BinaryOp>(
        std::make_shared<Variable>("x"),
        std::make_shared<Variable>("y"),
        BinaryOperator::Multiply
    );
    
    auto subExpr = std::make_shared<BinaryOp>(
        std::make_shared<Variable>("a"),
        std::make_shared<Variable>("b"),
        BinaryOperator::Subtract
    );
    
    auto addExpr1 = std::make_shared<BinaryOp>(
        mulExpr,
        subExpr,
        BinaryOperator::Add
    );
    
    auto addExpr2 = std::make_shared<BinaryOp>(
        std::make_shared<Variable>("c"),
        std::make_shared<Variable>("d"),
        BinaryOperator::Add
    );
    
    auto divExpr = std::make_shared<BinaryOp>(
        addExpr1,
        addExpr2,
        BinaryOperator::Divide
    );
    
    EXPECT_NEAR(divExpr->evaluate(vars), 27.0 / 7.0, 1e-10);
}

TEST(ExpressionTest, DifferenceOfSquares) {
    // (a + b) * (a - b) = a² - b²
    // a=5, b=3 => (5+3)*(5-3) = 8*2 = 16 = 25-9
    std::map<std::string, double> vars = {{"a", 5.0}, {"b", 3.0}};
    
    auto addExpr = std::make_shared<BinaryOp>(
        std::make_shared<Variable>("a"),
        std::make_shared<Variable>("b"),
        BinaryOperator::Add
    );
    
    auto subExpr = std::make_shared<BinaryOp>(
        std::make_shared<Variable>("a"),
        std::make_shared<Variable>("b"),
        BinaryOperator::Subtract
    );
    
    auto mulExpr = std::make_shared<BinaryOp>(
        addExpr,
        subExpr,
        BinaryOperator::Multiply
    );
    
    EXPECT_EQ(mulExpr->evaluate(vars), 16.0);
}
