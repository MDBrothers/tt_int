#include <gtest/gtest.h>
#include "expression_builder.h"
#include "distribution.h"
#include "variable_registry.h"
#include "monte_carlo_evaluator.h"
#include <cmath>

using namespace tt_int;

TEST(IntegrationTest, PortfolioValue) {
    // Portfolio: stock_price * quantity
    // stock_price ~ N(100, 15), quantity ~ U(10, 20)
    // Expected mean: 100 * 15 = 1500
    // Expected variance: E[price^2]*Var[qty] + E[qty]^2*Var[price] + Var[price]*Var[qty]
    
    VariableRegistry registry;
    registry.registerVariable("stock_price", 
        std::make_shared<NormalDistribution>(100.0, 15.0));
    registry.registerVariable("quantity", 
        std::make_shared<UniformDistribution>(10.0, 20.0));
    
    auto price = ExpressionBuilder::variable("stock_price");
    auto qty = ExpressionBuilder::variable("quantity");
    auto value = price * qty;
    
    MonteCarloEvaluator evaluator(10000, 42);
    auto result = evaluator.evaluate(value.get(), registry);
    
    // Mean should be around 100 * 15 = 1500
    EXPECT_NEAR(result.mean, 1500.0, 50.0);
    EXPECT_GT(result.stddev, 0.0);
    EXPECT_EQ(result.validSampleCount, 10000);
    EXPECT_EQ(result.totalSampleCount, 10000);
}

TEST(IntegrationTest, RatioWithDivideByZero) {
    // numerator ~ N(10, 2), denominator ~ N(0, 1)
    // Some denominator values will be very close to or equal to zero
    
    VariableRegistry registry;
    registry.registerVariable("numerator", 
        std::make_shared<NormalDistribution>(10.0, 2.0));
    registry.registerVariable("denominator", 
        std::make_shared<NormalDistribution>(0.0, 1.0));
    
    auto num = ExpressionBuilder::variable("numerator");
    auto den = ExpressionBuilder::variable("denominator");
    auto ratio = num / den;
    
    MonteCarloEvaluator evaluator(1000, 42);
    auto result = evaluator.evaluate(ratio.get(), registry);
    
    // Some samples should be NaN due to division by near-zero
    // (Note: exact zero is unlikely with continuous distribution, but very small values exist)
    EXPECT_LE(result.validSampleCount, result.totalSampleCount);
    EXPECT_EQ(result.totalSampleCount, 1000);
    
    // If there are valid samples, statistics should be reasonable
    if (result.validSampleCount > 0) {
        EXPECT_FALSE(std::isnan(result.mean));
        EXPECT_FALSE(std::isnan(result.stddev));
    }
}

TEST(IntegrationTest, ComplexFormula) {
    // (a + b) * c - a / b
    // a ~ N(5, 1), b ~ N(3, 0.5), c ~ U(1, 2)
    
    VariableRegistry registry;
    registry.registerVariable("a", std::make_shared<NormalDistribution>(5.0, 1.0));
    registry.registerVariable("b", std::make_shared<NormalDistribution>(3.0, 0.5));
    registry.registerVariable("c", std::make_shared<UniformDistribution>(1.0, 2.0));
    
    auto a = ExpressionBuilder::variable("a");
    auto b = ExpressionBuilder::variable("b");
    auto c = ExpressionBuilder::variable("c");
    
    auto expr = (a + b) * c - a / b;
    
    MonteCarloEvaluator evaluator(10000, 42);
    auto result = evaluator.evaluate(expr.get(), registry);
    
    // Rough expected value: (5 + 3) * 1.5 - 5/3 ≈ 12 - 1.67 ≈ 10.33
    EXPECT_NEAR(result.mean, 10.33, 1.0);
    EXPECT_GT(result.stddev, 0.0);
    EXPECT_EQ(result.validSampleCount, 10000);
}

TEST(IntegrationTest, RevenueCalculation) {
    // Revenue = price * demand
    // price ~ U(10, 20), demand ~ N(1000, 150)
    
    VariableRegistry registry;
    registry.registerVariable("price", 
        std::make_shared<UniformDistribution>(10.0, 20.0));
    registry.registerVariable("demand", 
        std::make_shared<NormalDistribution>(1000.0, 150.0));
    
    auto price = ExpressionBuilder::variable("price");
    auto demand = ExpressionBuilder::variable("demand");
    auto revenue = price * demand;
    
    MonteCarloEvaluator evaluator(10000, 123);
    auto result = evaluator.evaluate(revenue.get(), registry);
    
    // Expected mean: 15 * 1000 = 15000
    EXPECT_NEAR(result.mean, 15000.0, 500.0);
    EXPECT_GT(result.stddev, 0.0);
}

TEST(IntegrationTest, ProfitMargin) {
    // Profit margin = (revenue - cost) / revenue
    // revenue ~ N(100, 10), cost ~ N(70, 5)
    
    VariableRegistry registry;
    registry.registerVariable("revenue", 
        std::make_shared<NormalDistribution>(100.0, 10.0));
    registry.registerVariable("cost", 
        std::make_shared<NormalDistribution>(70.0, 5.0));
    
    auto revenue = ExpressionBuilder::variable("revenue");
    auto cost = ExpressionBuilder::variable("cost");
    auto margin = (revenue - cost) / revenue;
    
    MonteCarloEvaluator evaluator(10000, 456);
    auto result = evaluator.evaluate(margin.get(), registry);
    
    // Expected margin: (100 - 70) / 100 = 0.30
    EXPECT_NEAR(result.mean, 0.30, 0.05);
    EXPECT_GT(result.stddev, 0.0);
}

TEST(IntegrationTest, QuadraticFormula) {
    // Quadratic: a*x^2 + b*x + c
    // x ~ N(2, 0.5), a=1, b=3, c=2
    
    VariableRegistry registry;
    registry.registerVariable("x", std::make_shared<NormalDistribution>(2.0, 0.5));
    
    auto x = ExpressionBuilder::variable("x");
    auto a = ExpressionBuilder::constant(1.0);
    auto b = ExpressionBuilder::constant(3.0);
    auto c = ExpressionBuilder::constant(2.0);
    
    auto x2 = x * x;
    auto expr = a * x2 + b * x + c;
    
    MonteCarloEvaluator evaluator(10000, 789);
    auto result = evaluator.evaluate(expr.get(), registry);
    
    // For x=2: 1*4 + 3*2 + 2 = 4 + 6 + 2 = 12
    EXPECT_NEAR(result.mean, 12.0, 1.0);
    EXPECT_GT(result.stddev, 0.0);
}

TEST(IntegrationTest, ReturnOnInvestment) {
    // ROI = (gain - cost) / cost
    // gain ~ N(120, 20), cost ~ U(80, 100)
    
    VariableRegistry registry;
    registry.registerVariable("gain", 
        std::make_shared<NormalDistribution>(120.0, 20.0));
    registry.registerVariable("cost", 
        std::make_shared<UniformDistribution>(80.0, 100.0));
    
    auto gain = ExpressionBuilder::variable("gain");
    auto cost = ExpressionBuilder::variable("cost");
    auto roi = (gain - cost) / cost;
    
    MonteCarloEvaluator evaluator(10000, 321);
    auto result = evaluator.evaluate(roi.get(), registry);
    
    // Expected ROI: (120 - 90) / 90 ≈ 0.333
    EXPECT_NEAR(result.mean, 0.333, 0.1);
    EXPECT_GT(result.stddev, 0.0);
}

TEST(IntegrationTest, WeightedAverage) {
    // Weighted average: (w1*x1 + w2*x2) / (w1 + w2)
    // x1 ~ N(100, 10), x2 ~ N(80, 15), w1=2, w2=3
    
    VariableRegistry registry;
    registry.registerVariable("x1", std::make_shared<NormalDistribution>(100.0, 10.0));
    registry.registerVariable("x2", std::make_shared<NormalDistribution>(80.0, 15.0));
    
    auto x1 = ExpressionBuilder::variable("x1");
    auto x2 = ExpressionBuilder::variable("x2");
    
    auto weighted_avg = (2.0 * x1 + 3.0 * x2) / 5.0;
    
    MonteCarloEvaluator evaluator(10000, 654);
    auto result = evaluator.evaluate(weighted_avg.get(), registry);
    
    // Expected: (2*100 + 3*80) / 5 = (200 + 240) / 5 = 88
    EXPECT_NEAR(result.mean, 88.0, 2.0);
    EXPECT_GT(result.stddev, 0.0);
}

TEST(IntegrationTest, CompoundExpression) {
    // Complex real-world scenario: Portfolio with multiple assets
    // total_value = asset1 * price1 + asset2 * price2 - fees
    
    VariableRegistry registry;
    registry.registerVariable("asset1", std::make_shared<UniformDistribution>(10.0, 15.0));
    registry.registerVariable("price1", std::make_shared<NormalDistribution>(100.0, 10.0));
    registry.registerVariable("asset2", std::make_shared<UniformDistribution>(20.0, 30.0));
    registry.registerVariable("price2", std::make_shared<NormalDistribution>(50.0, 5.0));
    registry.registerVariable("fees", std::make_shared<UniformDistribution>(10.0, 20.0));
    
    auto asset1 = ExpressionBuilder::variable("asset1");
    auto price1 = ExpressionBuilder::variable("price1");
    auto asset2 = ExpressionBuilder::variable("asset2");
    auto price2 = ExpressionBuilder::variable("price2");
    auto fees = ExpressionBuilder::variable("fees");
    
    auto total_value = asset1 * price1 + asset2 * price2 - fees;
    
    MonteCarloEvaluator evaluator(10000, 987);
    auto result = evaluator.evaluate(total_value.get(), registry);
    
    // Expected: 12.5 * 100 + 25 * 50 - 15 = 1250 + 1250 - 15 = 2485
    EXPECT_NEAR(result.mean, 2485.0, 100.0);
    EXPECT_GT(result.stddev, 0.0);
    EXPECT_EQ(result.validSampleCount, 10000);
}
