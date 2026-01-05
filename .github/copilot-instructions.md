# GitHub Copilot Instructions for TT_INT Project

## Project Overview

This is a Monte Carlo Expression Calculator - a modern C++ project using:
- **C++ Standard**: C++17
- **Build System**: CMake 3.14+
- **Testing Framework**: Google Test (GTest)
- **Namespace**: `tt_int`
- **Core Features**: Expression trees, probability distributions, Monte Carlo simulation

## Code Style Guidelines

### Naming Conventions
- **Functions**: camelCase (e.g., `getGreeting`, `calculateSum`)
- **Variables**: camelCase (e.g., `userName`, `itemCount`)
- **Classes**: PascalCase (e.g., `MyClass`, `DataProcessor`)
- **Constants**: UPPER_SNAKE_CASE (e.g., `MAX_SIZE`, `DEFAULT_VALUE`)
- **Namespace**: snake_case (e.g., `tt_int`)

### File Organization
- **Headers**: Place in `include/` directory with `.h` extension
- **Implementation**: Place in `src/` directory with `.cpp` extension
- **Tests**: Place in `tests/` directory with `test_*.cpp` naming

### Header Guards
Use `#ifndef`/`#define` header guards with format: `FILENAME_H`

Example:
```cpp
#ifndef HELLO_H
#define HELLO_H
// ... content ...
#endif // HELLO_H
```

### Documentation
- Use Doxygen-style comments for public APIs
- Include `@brief`, `@param`, and `@return` tags
- Add comments for complex logic

Example:
```cpp
/**
 * @brief Calculates the sum of two integers
 * @param a First integer
 * @param b Second integer
 * @return The sum of a and b
 */
int add(int a, int b);
```

## Project Structure

### Adding New Components

When adding new functionality:
1. Create header in `include/`
2. Create implementation in `src/`
3. Create tests in `tests/`
4. Update `CMakeLists.txt` to include new files

### CMake Integration

When suggesting new source files:
- Add to `hello_lib` target for library code
- Add to `tt_int` target for application code
- Add to `tests` target for test code

## Testing Guidelines

### Test Structure
- Use descriptive test names: `TEST(ComponentName, BehaviorDescription)`
- Group related tests in the same test suite
- Use test fixtures for setup/teardown when needed

Example:
```cpp
TEST(CalculatorTest, AdditionWorksCorrectly) {
    EXPECT_EQ(add(2, 3), 5);
}
```

### Test Coverage
- Write tests for all public functions
- Include edge cases and error conditions
- Test both success and failure paths

## Best Practices

### Memory Management
- Prefer stack allocation over heap when possible
- Use smart pointers (`std::unique_ptr`, `std::shared_ptr`) for heap allocations
- Avoid raw `new`/`delete`

### Error Handling
- Use exceptions for exceptional conditions
- Return error codes for expected failures
- Always document exception specifications

### Modern C++ Features
- Use `auto` for complex type deductions
- Prefer range-based for loops
- Use `nullptr` instead of `NULL`
- Use `override` keyword for virtual function overrides

### Include Order
1. Corresponding header (for .cpp files)
2. C system headers
3. C++ standard library headers
4. Third-party library headers
5. Project headers

Example:
```cpp
#include "hello.h"

#include <iostream>
#include <string>
#include <vector>

#include "other_project_header.h"
```

## Common Patterns

### Function Implementation Template
```cpp
// In header file
namespace tt_int {
    /**
     * @brief Brief description
     * @param paramName Description
     * @return Description
     */
    ReturnType functionName(ParamType paramName);
}

// In source file
namespace tt_int {
    ReturnType functionName(ParamType paramName) {
        // Implementation
    }
}
```

### Test Template
```cpp
#include <gtest/gtest.h>
#include "header_to_test.h"

TEST(TestSuiteName, TestName) {
    // Arrange
    // Act
    // Assert
    EXPECT_EQ(actual, expected);
}
```

## Build Commands

When suggesting build commands, use:
```bash
# Configure
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build build

# Test
ctest --test-dir build --output-on-failure
```

## Context for Copilot

When working with this project:
- All code should be in the `tt_int` namespace
- Headers go in `include/`, implementations in `src/`
- Always suggest corresponding tests for new functions
- Keep C++17 compatibility in mind
- Follow the existing project structure and naming conventions

---

## Calculator API Patterns

### Expression Tree Construction

**Builder API (Recommended)**:
```cpp
auto x = ExpressionBuilder::variable("x");
auto y = ExpressionBuilder::variable("y");
auto expr = (x + y) * 2.0 - x / y;
```

**Direct Construction (Low-level)**:
```cpp
auto x = std::make_shared<Variable>("x");
auto y = std::make_shared<Variable>("y");
auto sum = std::make_shared<BinaryOp>(x, y, BinaryOperator::Add);
```

### Memory Management for Expression Trees

- Use `std::shared_ptr<Expression>` for tree nodes
- Allows natural sub-expression reuse: `auto x2 = x * x; auto expr = x2 + x2;`
- Builder API wraps expressions automatically
- Call `.get()` on ExpressionBuilder to access underlying `shared_ptr<Expression>`

### Statistical Code Guidelines

**Random Number Generation**:
```cpp
// Use std::mt19937 for RNG
std::mt19937 rng(seed);  // Deterministic
std::mt19937 rng(std::random_device{}());  // Non-deterministic

// Distributions are mutable (state changes during sampling)
mutable std::normal_distribution<double> dist_;
```

**Deterministic Seeding**:
```cpp
// For testing - use deterministic seed
MonteCarloEvaluator evaluator(10000, 42);

// For production - omit seed or use std::nullopt
MonteCarloEvaluator evaluator(10000);
```

**Online Statistics with Welford's Algorithm**:
```cpp
// Numerically stable running mean and variance
double mean = 0.0;
double M2 = 0.0;
size_t n = 0;

for (const auto& value : samples) {
    if (!std::isnan(value)) {
        ++n;
        double delta = value - mean;
        mean += delta / n;
        double delta2 = value - mean;
        M2 += delta * delta2;
    }
}

double variance = M2 / n;
double stddev = std::sqrt(variance);
```

### Error Handling

**Division by Zero**:
- Returns `std::numeric_limits<double>::quiet_NaN()`
- Does NOT throw exceptions
- Track valid vs. invalid samples separately

```cpp
double divisor = right->evaluate(variables);
if (std::abs(divisor) < 1e-10) {
    return std::numeric_limits<double>::quiet_NaN();
}
return numerator / divisor;
```

**NaN Propagation**:
```cpp
// Track NaN samples in results
size_t validCount = 0;
for (const auto& sample : samples) {
    if (!std::isnan(sample)) {
        ++validCount;
        // Include in statistics
    }
}
```

### Variable Registry Pattern

```cpp
VariableRegistry registry;

// Register variables with distributions
registry.registerVariable("price", 
    std::make_shared<NormalDistribution>(100.0, 15.0));
registry.registerVariable("quantity", 
    std::make_shared<UniformDistribution>(10.0, 20.0));

// Sample all variables at once
auto samples = registry.sampleAll(rng);  // Returns map<string, double>
```

### Testing Statistical Code

**Use Deterministic Seeds**:
```cpp
TEST(MonteCarloTest, SumOfNormals) {
    std::mt19937 rng(42);  // Fixed seed
    // ... test code
}
```

**Test Against Known Analytical Results**:
```cpp
// X ~ N(5, 2), Y ~ N(3, 1)
// X + Y ~ N(8, sqrt(5))
registry.registerVariable("x", std::make_shared<NormalDistribution>(5, 2));
registry.registerVariable("y", std::make_shared<NormalDistribution>(3, 1));

auto result = evaluator.evaluate(expr, registry);
EXPECT_NEAR(result.mean, 8.0, 0.15);  // Allow statistical tolerance
EXPECT_NEAR(result.stddev, 2.236, 0.15);
```

**Statistical Tolerance Guidelines**:
- Mean: ±3σ/√N for N samples
- For N=10000: tolerance ≈ ±0.03σ
- Use `EXPECT_NEAR()` instead of `EXPECT_EQ()` for floating-point comparisons
- Convergence tests: verify error decreases with more samples

**Test Convergence**:
```cpp
// Error should decrease roughly as 1/sqrt(N)
auto result_100 = evaluator_100.evaluate(expr, registry);
auto result_10000 = evaluator_10000.evaluate(expr, registry);

double error_100 = std::abs(result_100.mean - expected);
double error_10000 = std::abs(result_10000.mean - expected);

EXPECT_LT(error_10000, error_100);  // More samples = less error
```

### Operator Overloading Pattern

```cpp
// Member operators return new ExpressionBuilder
ExpressionBuilder ExpressionBuilder::operator+(const ExpressionBuilder& other) const {
    return ExpressionBuilder(
        std::make_shared<BinaryOp>(expr_, other.expr_, BinaryOperator::Add)
    );
}

// Free functions for mixed operations
ExpressionBuilder operator+(const ExpressionBuilder& lhs, double rhs) {
    return lhs + ExpressionBuilder::constant(rhs);
}

ExpressionBuilder operator+(double lhs, const ExpressionBuilder& rhs) {
    return ExpressionBuilder::constant(lhs) + rhs;
}
```

### Convergence Tracking

```cpp
// Enable convergence tracking
// 0 = disabled (default)
// positive = fixed interval
// -1 = smart intervals (logarithmic + percentage)
auto result = evaluator.evaluate(expr, registry, -1);

// Access convergence history
for (const auto& point : result.convergenceHistory) {
    double se = point.stddev / std::sqrt(point.validCount);
    std::cout << "n=" << point.sampleCount 
              << ", mean=" << point.mean 
              << ", SE=" << se << "\n";
}
```

### Distribution Factory Pattern

```cpp
// Normal: N(mean, stddev)
auto normal = std::make_shared<NormalDistribution>(100.0, 15.0);

// Uniform: U(min, max)
auto uniform = std::make_shared<UniformDistribution>(0.0, 10.0);

// Both inherit from Distribution base class
std::shared_ptr<Distribution> dist = /* either type */;
double sample = dist->sample(rng);
```

---

## Common Calculator Use Cases

### Financial Modeling
```cpp
// Portfolio value
auto value = price * quantity;

// Return on Investment
auto roi = (gain - cost) / cost;

// Sharpe Ratio
auto sharpe = (returns - riskFreeRate) / volatility;
```

### Physics/Engineering
```cpp
// Pythagorean distance
auto distance = x * x + y * y;

// Kinetic energy
auto energy = 0.5 * mass * velocity * velocity;
```

### Statistics
```cpp
// Weighted average
auto weighted = (w1 * x1 + w2 * x2) / (w1 + w2);

// Z-score
auto zscore = (x - mean) / stddev;
```
