/**
 * @file calculator_demo.cpp
 * @brief Standalone demo of Monte Carlo Expression Calculator
 * 
 * This example demonstrates the key features of the calculator:
 * - Variable registration with probability distributions
 * - Expression building with natural C++ syntax
 * - Monte Carlo simulation
 * - Statistical result interpretation
 * 
 * Compile:
 *   g++ -std=c++17 -I../include examples/calculator_demo.cpp \
 *       src/expression.cpp src/expression_builder.cpp \
 *       src/distribution.cpp src/variable_registry.cpp \
 *       src/monte_carlo_evaluator.cpp -o calculator_demo
 * 
 * Run:
 *   ./calculator_demo
 */

#include <iostream>
#include <iomanip>
#include "expression_builder.h"
#include "distribution.h"
#include "variable_registry.h"
#include "monte_carlo_evaluator.h"

using namespace tt_int;

void printSeparator() {
    std::cout << "\n" << std::string(70, '=') << "\n\n";
}

/**
 * Example 1: Portfolio Value Calculation
 * 
 * Calculate the total value of a stock portfolio where:
 * - Stock price follows a normal distribution (mean=$100, std dev=$15)
 * - Quantity held follows a uniform distribution (10-20 shares)
 */
void portfolioExample() {
    std::cout << "Example 1: Portfolio Value Calculation\n";
    std::cout << std::string(70, '-') << "\n\n";
    
    // Setup distributions
    VariableRegistry registry;
    registry.registerVariable("stock_price", 
        std::make_shared<NormalDistribution>(100.0, 15.0));
    registry.registerVariable("quantity", 
        std::make_shared<UniformDistribution>(10.0, 20.0));
    
    // Build expression
    auto price = ExpressionBuilder::variable("stock_price");
    auto qty = ExpressionBuilder::variable("quantity");
    auto portfolioValue = price * qty;
    
    std::cout << "Variables:\n";
    std::cout << "  stock_price ~ Normal(100, 15)  [dollars]\n";
    std::cout << "  quantity ~ Uniform(10, 20)     [shares]\n\n";
    std::cout << "Expression: portfolio_value = stock_price * quantity\n\n";
    
    // Run simulation
    MonteCarloEvaluator evaluator(10000, 42);
    auto result = evaluator.evaluate(portfolioValue.get(), registry);
    
    std::cout << "Results (10,000 simulations):\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  Mean:   $" << result.mean << "\n";
    std::cout << "  StdDev: $" << result.stddev << "\n";
    std::cout << "  Range:  $" << result.min << " - $" << result.max << "\n";
    std::cout << "  Valid samples: " << result.validSampleCount 
              << " / " << result.totalSampleCount << "\n";
}

/**
 * Example 2: Return on Investment (ROI)
 * 
 * Calculate ROI = (profit - investment) / investment
 * Both profit and investment are uncertain (follow normal distributions)
 */
void roiExample() {
    std::cout << "Example 2: Return on Investment\n";
    std::cout << std::string(70, '-') << "\n\n";
    
    VariableRegistry registry;
    registry.registerVariable("profit", 
        std::make_shared<NormalDistribution>(12.0, 3.0));
    registry.registerVariable("investment", 
        std::make_shared<NormalDistribution>(50.0, 5.0));
    
    auto profit = ExpressionBuilder::variable("profit");
    auto investment = ExpressionBuilder::variable("investment");
    auto roi = (profit - investment) / investment;
    
    std::cout << "Variables:\n";
    std::cout << "  profit ~ Normal(12, 3)       [thousands]\n";
    std::cout << "  investment ~ Normal(50, 5)   [thousands]\n\n";
    std::cout << "Expression: ROI = (profit - investment) / investment\n\n";
    
    MonteCarloEvaluator evaluator(10000, 123);
    auto result = evaluator.evaluate(roi.get(), registry);
    
    std::cout << "Results (10,000 simulations):\n";
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "  Mean ROI:   " << (result.mean * 100) << "%\n";
    std::cout << "  Std Dev:    " << (result.stddev * 100) << "%\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  95% CI:     " << (result.mean - 1.96 * result.stddev) * 100 
              << "% to " << (result.mean + 1.96 * result.stddev) * 100 << "%\n";
}

/**
 * Example 3: Complex Expression with Expression Reuse
 * 
 * Calculate weighted average with sub-expression reuse
 */
void weightedAverageExample() {
    std::cout << "Example 3: Weighted Average (Expression Reuse)\n";
    std::cout << std::string(70, '-') << "\n\n";
    
    VariableRegistry registry;
    registry.registerVariable("x1", std::make_shared<NormalDistribution>(100.0, 10.0));
    registry.registerVariable("x2", std::make_shared<NormalDistribution>(80.0, 15.0));
    
    // Build expression with reuse
    auto x1 = ExpressionBuilder::variable("x1");
    auto x2 = ExpressionBuilder::variable("x2");
    auto w1 = ExpressionBuilder::constant(0.6);
    auto w2 = ExpressionBuilder::constant(0.4);
    
    auto weightedAvg = w1 * x1 + w2 * x2;
    
    std::cout << "Variables:\n";
    std::cout << "  x1 ~ Normal(100, 10)\n";
    std::cout << "  x2 ~ Normal(80, 15)\n\n";
    std::cout << "Expression: weighted_avg = 0.6 * x1 + 0.4 * x2\n\n";
    
    MonteCarloEvaluator evaluator(10000, 456);
    auto result = evaluator.evaluate(weightedAvg.get(), registry);
    
    std::cout << "Results (10,000 simulations):\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  Mean:   " << result.mean << "\n";
    std::cout << "  StdDev: " << result.stddev << "\n";
    std::cout << "  Expected (analytical): " << (0.6 * 100 + 0.4 * 80) << "\n";
}

/**
 * Example 4: Convergence Tracking
 * 
 * Demonstrate how to track convergence of simulation statistics
 */
void convergenceExample() {
    std::cout << "Example 4: Convergence Tracking\n";
    std::cout << std::string(70, '-') << "\n\n";
    
    VariableRegistry registry;
    registry.registerVariable("demand", 
        std::make_shared<NormalDistribution>(1000.0, 200.0));
    registry.registerVariable("price", 
        std::make_shared<UniformDistribution>(10.0, 20.0));
    
    auto demand = ExpressionBuilder::variable("demand");
    auto price = ExpressionBuilder::variable("price");
    auto revenue = demand * price;
    
    std::cout << "Variables:\n";
    std::cout << "  demand ~ Normal(1000, 200)  [units]\n";
    std::cout << "  price ~ Uniform(10, 20)     [dollars]\n\n";
    std::cout << "Expression: revenue = demand * price\n\n";
    
    // Enable convergence tracking with smart intervals (-1)
    MonteCarloEvaluator evaluator(10000, 789);
    auto result = evaluator.evaluate(revenue.get(), registry, -1);
    
    std::cout << "Convergence History:\n";
    std::cout << std::string(70, '-') << "\n";
    std::cout << std::setw(12) << "Samples" 
              << std::setw(15) << "Mean" 
              << std::setw(15) << "Std Error"
              << std::setw(15) << "% Change"
              << "\n";
    std::cout << std::string(70, '-') << "\n";
    
    std::cout << std::fixed << std::setprecision(2);
    double prevMean = 0.0;
    for (const auto& point : result.convergenceHistory) {
        double se = point.stddev / std::sqrt(point.validCount);
        double changePercent = (prevMean == 0.0) ? 0.0 : 
            std::abs(point.mean - prevMean) / prevMean * 100.0;
        
        std::cout << std::setw(12) << point.sampleCount
                  << std::setw(15) << point.mean
                  << std::setw(15) << se
                  << std::setw(14) << changePercent << "%"
                  << "\n";
        
        prevMean = point.mean;
    }
    
    std::cout << "\nFinal Result:\n";
    std::cout << "  Mean Revenue: $" << result.mean << "\n";
    std::cout << "  Expected: ~$" << (1000.0 * 15.0) << " (analytical)\n";
}

/**
 * Example 5: Mixed Operations and Complex Formulas
 * 
 * Demonstrate natural C++ syntax with mixed builder/constant operations
 */
void complexFormulaExample() {
    std::cout << "Example 5: Complex Formula (Quadratic)\n";
    std::cout << std::string(70, '-') << "\n\n";
    
    VariableRegistry registry;
    registry.registerVariable("x", std::make_shared<NormalDistribution>(2.0, 0.5));
    
    auto x = ExpressionBuilder::variable("x");
    
    // Quadratic: f(x) = x^2 + 3x + 2
    auto xSquared = x * x;
    auto formula = xSquared + 3.0 * x + 2.0;
    
    std::cout << "Variable:\n";
    std::cout << "  x ~ Normal(2, 0.5)\n\n";
    std::cout << "Expression: f(x) = x^2 + 3x + 2\n\n";
    
    MonteCarloEvaluator evaluator(10000, 321);
    auto result = evaluator.evaluate(formula.get(), registry);
    
    std::cout << "Results (10,000 simulations):\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  Mean:   " << result.mean << "\n";
    std::cout << "  StdDev: " << result.stddev << "\n";
    std::cout << "  f(2) = " << (4 + 6 + 2) << " (exact value at mean)\n";
}

int main() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║       Monte Carlo Expression Calculator - Standalone Demo           ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════════════╝\n";
    
    printSeparator();
    portfolioExample();
    
    printSeparator();
    roiExample();
    
    printSeparator();
    weightedAverageExample();
    
    printSeparator();
    convergenceExample();
    
    printSeparator();
    complexFormulaExample();
    
    printSeparator();
    std::cout << "✅ Demo Complete!\n\n";
    std::cout << "Key Takeaways:\n";
    std::cout << "  • Natural C++ syntax with operator overloading\n";
    std::cout << "  • Support for Normal and Uniform distributions\n";
    std::cout << "  • Monte Carlo simulation with statistical results\n";
    std::cout << "  • Convergence tracking for simulation quality assessment\n";
    std::cout << "  • Expression reuse and composition\n\n";
    
    return 0;
}
