#include <gtest/gtest.h>
#include "expression_builder.h"
#include <cmath>
#include <map>

using namespace tt_int;

TEST(BuilderTest, ConstantCreation) {
    auto builder = ExpressionBuilder::constant(42.0);
    auto expr = builder.get();
    
    std::map<std::string, double> variables;
    double result = expr->evaluate(variables);
    
    EXPECT_EQ(result, 42.0);
}

TEST(BuilderTest, VariableCreation) {
    auto builder = ExpressionBuilder::variable("x");
    auto expr = builder.get();
    
    std::map<std::string, double> variables = {{"x", 5.0}};
    double result = expr->evaluate(variables);
    
    EXPECT_EQ(result, 5.0);
}

TEST(BuilderTest, AdditionOperator) {
    auto x = ExpressionBuilder::variable("x");
    auto y = ExpressionBuilder::variable("y");
    auto sum = x + y;
    
    std::map<std::string, double> variables = {{"x", 3.0}, {"y", 7.0}};
    double result = sum.get()->evaluate(variables);
    
    EXPECT_EQ(result, 10.0);
}

TEST(BuilderTest, SubtractionOperator) {
    auto x = ExpressionBuilder::variable("x");
    auto y = ExpressionBuilder::variable("y");
    auto diff = x - y;
    
    std::map<std::string, double> variables = {{"x", 10.0}, {"y", 3.0}};
    double result = diff.get()->evaluate(variables);
    
    EXPECT_EQ(result, 7.0);
}

TEST(BuilderTest, MultiplicationOperator) {
    auto x = ExpressionBuilder::variable("x");
    auto y = ExpressionBuilder::variable("y");
    auto product = x * y;
    
    std::map<std::string, double> variables = {{"x", 4.0}, {"y", 5.0}};
    double result = product.get()->evaluate(variables);
    
    EXPECT_EQ(result, 20.0);
}

TEST(BuilderTest, DivisionOperator) {
    auto x = ExpressionBuilder::variable("x");
    auto y = ExpressionBuilder::variable("y");
    auto quotient = x / y;
    
    std::map<std::string, double> variables = {{"x", 15.0}, {"y", 3.0}};
    double result = quotient.get()->evaluate(variables);
    
    EXPECT_EQ(result, 5.0);
}

TEST(BuilderTest, MixedBuilderDouble) {
    auto x = ExpressionBuilder::variable("x");
    auto expr = x + 5.0;
    
    std::map<std::string, double> variables = {{"x", 10.0}};
    double result = expr.get()->evaluate(variables);
    
    EXPECT_EQ(result, 15.0);
}

TEST(BuilderTest, MixedDoubleBuilder) {
    auto x = ExpressionBuilder::variable("x");
    auto expr = 5.0 + x;
    
    std::map<std::string, double> variables = {{"x", 10.0}};
    double result = expr.get()->evaluate(variables);
    
    EXPECT_EQ(result, 15.0);
}

TEST(BuilderTest, MixedSubtraction) {
    auto x = ExpressionBuilder::variable("x");
    auto expr1 = x - 3.0;
    auto expr2 = 10.0 - x;
    
    std::map<std::string, double> variables = {{"x", 7.0}};
    
    EXPECT_EQ(expr1.get()->evaluate(variables), 4.0);
    EXPECT_EQ(expr2.get()->evaluate(variables), 3.0);
}

TEST(BuilderTest, MixedMultiplication) {
    auto x = ExpressionBuilder::variable("x");
    auto expr1 = x * 2.0;
    auto expr2 = 2.0 * x;
    
    std::map<std::string, double> variables = {{"x", 5.0}};
    
    EXPECT_EQ(expr1.get()->evaluate(variables), 10.0);
    EXPECT_EQ(expr2.get()->evaluate(variables), 10.0);
}

TEST(BuilderTest, MixedDivision) {
    auto x = ExpressionBuilder::variable("x");
    auto expr1 = x / 2.0;
    auto expr2 = 10.0 / x;
    
    std::map<std::string, double> variables = {{"x", 4.0}};
    
    EXPECT_EQ(expr1.get()->evaluate(variables), 2.0);
    EXPECT_EQ(expr2.get()->evaluate(variables), 2.5);
}

TEST(BuilderTest, ComplexExpression) {
    auto x = ExpressionBuilder::variable("x");
    auto y = ExpressionBuilder::variable("y");
    
    // (x + y) * (x - y) = x^2 - y^2
    auto expr = (x + y) * (x - y);
    
    std::map<std::string, double> variables = {{"x", 5.0}, {"y", 3.0}};
    double result = expr.get()->evaluate(variables);
    
    // 5^2 - 3^2 = 25 - 9 = 16
    EXPECT_EQ(result, 16.0);
}

TEST(BuilderTest, ExpressionReuse) {
    auto x = ExpressionBuilder::variable("x");
    auto x2 = x * x;  // x^2
    auto expr = x2 + x2;  // 2*x^2
    
    std::map<std::string, double> variables = {{"x", 3.0}};
    double result = expr.get()->evaluate(variables);
    
    // 2 * 3^2 = 2 * 9 = 18
    EXPECT_EQ(result, 18.0);
}

TEST(BuilderTest, NestedMixedOperations) {
    auto x = ExpressionBuilder::variable("x");
    
    // ((x + 5) * 2) - 3
    auto expr = (x + 5.0) * 2.0 - 3.0;
    
    std::map<std::string, double> variables = {{"x", 10.0}};
    double result = expr.get()->evaluate(variables);
    
    // ((10 + 5) * 2) - 3 = (15 * 2) - 3 = 30 - 3 = 27
    EXPECT_EQ(result, 27.0);
}

TEST(BuilderTest, ComplexFormulaWithConstants) {
    auto x = ExpressionBuilder::variable("x");
    auto y = ExpressionBuilder::variable("y");
    
    // x^2 + 2*x*y + y^2 = (x + y)^2
    auto x2 = x * x;
    auto y2 = y * y;
    auto twoXY = 2.0 * x * y;
    auto expr = x2 + twoXY + y2;
    
    std::map<std::string, double> variables = {{"x", 3.0}, {"y", 4.0}};
    double result = expr.get()->evaluate(variables);
    
    // (3 + 4)^2 = 49
    EXPECT_EQ(result, 49.0);
}

TEST(BuilderTest, DivisionByZero) {
    auto x = ExpressionBuilder::variable("x");
    auto expr = x / 0.0;
    
    std::map<std::string, double> variables = {{"x", 10.0}};
    double result = expr.get()->evaluate(variables);
    
    EXPECT_TRUE(std::isnan(result));
}

TEST(BuilderTest, ChainedOperations) {
    auto a = ExpressionBuilder::variable("a");
    auto b = ExpressionBuilder::variable("b");
    auto c = ExpressionBuilder::variable("c");
    
    // ((a + b) * c) - (a / b)
    auto expr = (a + b) * c - a / b;
    
    std::map<std::string, double> variables = {
        {"a", 6.0},
        {"b", 2.0},
        {"c", 5.0}
    };
    double result = expr.get()->evaluate(variables);
    
    // ((6 + 2) * 5) - (6 / 2) = (8 * 5) - 3 = 40 - 3 = 37
    EXPECT_EQ(result, 37.0);
}
