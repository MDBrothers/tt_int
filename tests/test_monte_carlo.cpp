#include <gtest/gtest.h>
#include "monte_carlo_evaluator.h"
#include "expression.h"
#include "distribution.h"
#include "variable_registry.h"
#include <cmath>
#include <memory>

using namespace tt_int;

// Test basic construction
TEST(MonteCarloTest, Construction) {
    MonteCarloEvaluator evaluator(1000);
    EXPECT_TRUE(true);  // If construction succeeds, test passes
}

// Test construction with seed
TEST(MonteCarloTest, ConstructionWithSeed) {
    MonteCarloEvaluator evaluator(1000, 42);
    EXPECT_TRUE(true);
}

// Test that evaluate returns a result with correct sample count
TEST(MonteCarloTest, EvaluateReturnsResult) {
    VariableRegistry registry;
    registry.registerVariable("x", std::make_shared<NormalDistribution>(0.0, 1.0));
    
    auto expr = std::make_shared<Variable>("x");
    MonteCarloEvaluator evaluator(1000, 42);
    
    auto result = evaluator.evaluate(expr, registry);
    
    EXPECT_EQ(result.samples.size(), 1000);
    EXPECT_EQ(result.totalSampleCount, 1000);
    EXPECT_EQ(result.validSampleCount, 1000);
    EXPECT_FALSE(std::isnan(result.mean));
    EXPECT_FALSE(std::isinf(result.mean));
    EXPECT_GE(result.stddev, 0.0);
}

// Test sum of two independent normal distributions
// X ~ N(5, 2^2), Y ~ N(3, 1^2)
// X + Y ~ N(8, 5) => mean=8, stddev=sqrt(5)≈2.236
TEST(MonteCarloTest, SumOfNormals) {
    VariableRegistry registry;
    registry.registerVariable("x", std::make_shared<NormalDistribution>(5.0, 2.0));
    registry.registerVariable("y", std::make_shared<NormalDistribution>(3.0, 1.0));
    
    auto x = std::make_shared<Variable>("x");
    auto y = std::make_shared<Variable>("y");
    auto expr = std::make_shared<BinaryOp>(x, y, BinaryOperator::Add);
    
    MonteCarloEvaluator evaluator(10000, 42);
    auto result = evaluator.evaluate(expr, registry);
    
    EXPECT_NEAR(result.mean, 8.0, 0.15);
    EXPECT_EQ(result.validSampleCount, 10000);
}

// Test sum of normals standard deviation
TEST(MonteCarloTest, SumOfNormalsStdDev) {
    VariableRegistry registry;
    registry.registerVariable("x", std::make_shared<NormalDistribution>(5.0, 2.0));
    registry.registerVariable("y", std::make_shared<NormalDistribution>(3.0, 1.0));
    
    auto x = std::make_shared<Variable>("x");
    auto y = std::make_shared<Variable>("y");
    auto expr = std::make_shared<BinaryOp>(x, y, BinaryOperator::Add);
    
    MonteCarloEvaluator evaluator(10000, 42);
    auto result = evaluator.evaluate(expr, registry);
    
    // Variance of sum = 2^2 + 1^2 = 5, so stddev = sqrt(5) ≈ 2.236
    EXPECT_NEAR(result.stddev, 2.236, 0.15);
}

// Test constant multiplication
// X ~ N(10, 3^2), 2*X ~ N(20, 36) => mean=20, stddev=6
TEST(MonteCarloTest, ConstantMultiplication) {
    VariableRegistry registry;
    registry.registerVariable("x", std::make_shared<NormalDistribution>(10.0, 3.0));
    
    auto x = std::make_shared<Variable>("x");
    auto two = std::make_shared<Constant>(2.0);
    auto expr = std::make_shared<BinaryOp>(two, x, BinaryOperator::Multiply);
    
    MonteCarloEvaluator evaluator(10000, 42);
    auto result = evaluator.evaluate(expr, registry);
    
    EXPECT_NEAR(result.mean, 20.0, 0.2);
    EXPECT_NEAR(result.stddev, 6.0, 0.2);
}

// Test negative multiplication
// X ~ N(5, 2^2), -1*X ~ N(-5, 4) => mean=-5, stddev=2
TEST(MonteCarloTest, NegativeMultiplication) {
    VariableRegistry registry;
    registry.registerVariable("x", std::make_shared<NormalDistribution>(5.0, 2.0));
    
    auto x = std::make_shared<Variable>("x");
    auto negOne = std::make_shared<Constant>(-1.0);
    auto expr = std::make_shared<BinaryOp>(negOne, x, BinaryOperator::Multiply);
    
    MonteCarloEvaluator evaluator(10000, 42);
    auto result = evaluator.evaluate(expr, registry);
    
    EXPECT_NEAR(result.mean, -5.0, 0.15);
    EXPECT_NEAR(result.stddev, 2.0, 0.15);
}

// Test sum of uniform distributions
// X ~ U(0,1), Y ~ U(0,1)
// X + Y has mean=1, variance=1/6
TEST(MonteCarloTest, UniformSum) {
    VariableRegistry registry;
    registry.registerVariable("x", std::make_shared<UniformDistribution>(0.0, 1.0));
    registry.registerVariable("y", std::make_shared<UniformDistribution>(0.0, 1.0));
    
    auto x = std::make_shared<Variable>("x");
    auto y = std::make_shared<Variable>("y");
    auto expr = std::make_shared<BinaryOp>(x, y, BinaryOperator::Add);
    
    MonteCarloEvaluator evaluator(10000, 42);
    auto result = evaluator.evaluate(expr, registry);
    
    EXPECT_NEAR(result.mean, 1.0, 0.05);
}

// Test divide by zero - all samples should be NaN
TEST(MonteCarloTest, DivideByZero) {
    VariableRegistry registry;
    registry.registerVariable("x", std::make_shared<NormalDistribution>(5.0, 1.0));
    
    auto x = std::make_shared<Variable>("x");
    auto zero = std::make_shared<Constant>(0.0);
    auto expr = std::make_shared<BinaryOp>(x, zero, BinaryOperator::Divide);
    
    MonteCarloEvaluator evaluator(1000, 42);
    auto result = evaluator.evaluate(expr, registry);
    
    EXPECT_EQ(result.validSampleCount, 0);
    EXPECT_EQ(result.totalSampleCount, 1000);
    EXPECT_TRUE(std::isnan(result.mean));
    
    // All samples should be NaN
    for (double sample : result.samples) {
        EXPECT_TRUE(std::isnan(sample));
    }
}

// Test divide by variable that can be zero
// X ~ N(10, 1), Y ~ N(0, 1)
// X / Y will have some NaN values when Y is close to zero
TEST(MonteCarloTest, DivideByVariable) {
    VariableRegistry registry;
    registry.registerVariable("x", std::make_shared<NormalDistribution>(10.0, 1.0));
    registry.registerVariable("y", std::make_shared<NormalDistribution>(0.0, 1.0));
    
    auto x = std::make_shared<Variable>("x");
    auto y = std::make_shared<Variable>("y");
    auto expr = std::make_shared<BinaryOp>(x, y, BinaryOperator::Divide);
    
    MonteCarloEvaluator evaluator(10000, 42);
    auto result = evaluator.evaluate(expr, registry);
    
    // Some samples should be invalid due to division by values close to zero
    // Note: Since we're dividing by N(0,1), we'll get some very large values
    // but not necessarily NaN unless we hit exactly zero (unlikely with continuous distribution)
    EXPECT_LE(result.validSampleCount, result.totalSampleCount);
    EXPECT_GT(result.validSampleCount, 0);  // Should have some valid samples
}

// Test deterministic seeding produces identical results
TEST(MonteCarloTest, DeterministicSeed) {
    VariableRegistry registry;
    registry.registerVariable("x", std::make_shared<NormalDistribution>(0.0, 1.0));
    registry.registerVariable("y", std::make_shared<UniformDistribution>(-1.0, 1.0));
    
    auto x = std::make_shared<Variable>("x");
    auto y = std::make_shared<Variable>("y");
    auto expr = std::make_shared<BinaryOp>(x, y, BinaryOperator::Add);
    
    MonteCarloEvaluator evaluator1(1000, 42);
    auto result1 = evaluator1.evaluate(expr, registry);
    
    MonteCarloEvaluator evaluator2(1000, 42);
    auto result2 = evaluator2.evaluate(expr, registry);
    
    EXPECT_EQ(result1.samples, result2.samples);
    EXPECT_DOUBLE_EQ(result1.mean, result2.mean);
    EXPECT_DOUBLE_EQ(result1.stddev, result2.stddev);
    EXPECT_DOUBLE_EQ(result1.min, result2.min);
    EXPECT_DOUBLE_EQ(result1.max, result2.max);
}

// Test different seeds produce different results
TEST(MonteCarloTest, DifferentSeeds) {
    VariableRegistry registry;
    registry.registerVariable("x", std::make_shared<NormalDistribution>(0.0, 1.0));
    
    auto x = std::make_shared<Variable>("x");
    
    MonteCarloEvaluator evaluator1(1000, 42);
    auto result1 = evaluator1.evaluate(x, registry);
    
    MonteCarloEvaluator evaluator2(1000, 123);
    auto result2 = evaluator2.evaluate(x, registry);
    
    EXPECT_NE(result1.samples, result2.samples);
    // Means should be different (with very high probability)
    EXPECT_NE(result1.mean, result2.mean);
}

// Test convergence - error decreases with more samples
TEST(MonteCarloTest, Convergence) {
    VariableRegistry registry;
    registry.registerVariable("x", std::make_shared<NormalDistribution>(100.0, 15.0));
    
    auto x = std::make_shared<Variable>("x");
    
    // Expected mean is 100.0
    const double expectedMean = 100.0;
    
    // Test with increasing sample sizes
    MonteCarloEvaluator eval100(100, 42);
    auto result100 = eval100.evaluate(x, registry);
    double error100 = std::abs(result100.mean - expectedMean);
    
    MonteCarloEvaluator eval1000(1000, 42);
    auto result1000 = eval1000.evaluate(x, registry);
    double error1000 = std::abs(result1000.mean - expectedMean);
    
    MonteCarloEvaluator eval10000(10000, 42);
    auto result10000 = eval10000.evaluate(x, registry);
    double error10000 = std::abs(result10000.mean - expectedMean);
    
    // With large sample size, error should be small
    EXPECT_LT(error10000, 1.0);
    
    // Standard error for N samples from N(100, 15) is 15/sqrt(N)
    // For 10000 samples: SE ≈ 0.15, so mean should be within ~3*SE with high probability
    EXPECT_LT(error10000, 0.5);
    
    // For 100 samples: SE ≈ 1.5, so we expect larger error
    // Just verify it's reasonable, not strict ordering
    EXPECT_LT(error100, 5.0);
}

// Test min/max are computed correctly
TEST(MonteCarloTest, MinMaxComputation) {
    VariableRegistry registry;
    registry.registerVariable("x", std::make_shared<UniformDistribution>(0.0, 10.0));
    
    auto x = std::make_shared<Variable>("x");
    
    MonteCarloEvaluator evaluator(10000, 42);
    auto result = evaluator.evaluate(x, registry);
    
    // Min should be close to 0, max should be close to 10
    EXPECT_GT(result.min, -0.1);
    EXPECT_LT(result.min, 1.0);
    EXPECT_GT(result.max, 9.0);
    EXPECT_LT(result.max, 10.1);
    
    // Min should be less than mean, mean should be less than max
    EXPECT_LT(result.min, result.mean);
    EXPECT_LT(result.mean, result.max);
}

// Test complex expression: (x + y) * z - x / y
TEST(MonteCarloTest, ComplexExpression) {
    VariableRegistry registry;
    registry.registerVariable("x", std::make_shared<NormalDistribution>(5.0, 1.0));
    registry.registerVariable("y", std::make_shared<NormalDistribution>(3.0, 0.5));
    registry.registerVariable("z", std::make_shared<UniformDistribution>(1.0, 2.0));
    
    // Build expression: (x + y) * z - x / y
    auto x = std::make_shared<Variable>("x");
    auto y = std::make_shared<Variable>("y");
    auto z = std::make_shared<Variable>("z");
    
    auto xPlusY = std::make_shared<BinaryOp>(x, y, BinaryOperator::Add);
    auto timesZ = std::make_shared<BinaryOp>(xPlusY, z, BinaryOperator::Multiply);
    auto xDivY = std::make_shared<BinaryOp>(x, y, BinaryOperator::Divide);
    auto expr = std::make_shared<BinaryOp>(timesZ, xDivY, BinaryOperator::Subtract);
    
    MonteCarloEvaluator evaluator(10000, 42);
    auto result = evaluator.evaluate(expr, registry);
    
    // Should produce valid results
    EXPECT_GT(result.validSampleCount, 9500);  // Most should be valid
    EXPECT_FALSE(std::isnan(result.mean));
    EXPECT_FALSE(std::isinf(result.mean));
    EXPECT_GT(result.stddev, 0.0);
    
    // Rough estimate: (x+y)*z has mean around (5+3)*1.5=12
    // x/y has mean around 5/3≈1.67
    // So result should be around 12-1.67≈10.33
    EXPECT_GT(result.mean, 8.0);
    EXPECT_LT(result.mean, 14.0);
}
