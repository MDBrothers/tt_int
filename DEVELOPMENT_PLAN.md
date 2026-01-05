# Monte Carlo Expression Calculator - Development Plan

## Progress Status

**Current Phase**: 🎉 PROJECT COMPLETE! All phases finished.

**Completed Phases**:

- ✅ **Phase 1**: Core Expression Tree (14 tests passing)
- ✅ **Phase 2**: Distribution System (17 tests passing)
- ✅ **Phase 3**: Monte Carlo Evaluator (23 tests passing)
  - Includes convergence tracking & introspection (Task 3.3)
- ✅ **Phase 4**: Expression Builder API (26 tests passing)
  - Natural operator overloading with mixed operations
- ✅ **Phase 5**: Project Integration & Documentation
  - Comprehensive README, Copilot instructions, standalone example

**Total Tests**: 84 passing (14 expression + 4 hello + 17 distribution + 23 monte carlo + 17 builder + 9 integration)

**Implementation Status**:

- ✅ Expression tree with 4 arithmetic operations
- ✅ Normal and Uniform distributions
- ✅ Variable registry for stochastic variables
- ✅ Monte Carlo evaluator with Welford's algorithm
- ✅ Convergence tracking with smart intervals
- ✅ Expression builder API with operator overloading
- ✅ Project integration & documentation (README, examples, Copilot instructions)

---

## Project Overview

Build a C++ expression tree calculator that supports statistical distributions (normal/uniform) and evaluates expressions using Monte Carlo simulation. The API allows users to define variables with probability distributions and compute statistical results from basic arithmetic expressions.

### Key Features

- Expression tree evaluation with variables
- Normal and uniform distribution support using C++ standard library
- Monte Carlo simulation engine
- Fluent C++ API with operator overloading
- Four basic arithmetic operations: `+`, `-`, `*`, `/`

### Design Decisions

#### Divide-by-Zero Handling

**Decision**: Return `std::numeric_limits<double>::quiet_NaN()` for division by zero

- Track count of invalid samples in `SimulationResult`
- Provides graceful degradation in statistical context
- Allows simulation to continue rather than throwing exceptions

#### Random Number Generator Seeding

**Decision**: Support optional deterministic seeding

- Constructor signature: `MonteCarloEvaluator(size_t numSamples, std::optional<unsigned> seed = std::nullopt)`
- Essential for reproducible testing
- Default behavior uses non-deterministic seed

#### Expression Tree Memory Management

**Decision**: Use `std::shared_ptr<Expression>` for tree nodes

- Allows natural sub-expression reuse (e.g., `auto x2 = x * x; auto result = x2 + x2;`)
- Provides flexible ownership semantics
- Simplifies builder API implementation

#### Optimization Strategy

**Decision**: No parallelization in initial implementation

- Keep implementation simple and maintainable
- Can add parallel evaluation in future if needed
- Focus on correctness and API design first

#### Mathematical Functions

**Decision**: Only four basic arithmetic operations

- No transcendental functions (sin, cos, exp, log, etc.)
- Supports: addition, subtraction, multiplication, division
- Keeps initial scope manageable

---

## Phase 1: Core Expression Tree (Deterministic Evaluation) ✅ COMPLETE

### Objective

Build expression tree infrastructure that evaluates with fixed variable values.

### Tasks

#### 1.1 Create Basic Expression Infrastructure

**Files**: `include/expression.h`, `src/expression.cpp`

```cpp
// Abstract base class
class Expression {
public:
    virtual ~Expression() = default;
    virtual double evaluate(const std::map<std::string, double>& variables) const = 0;
};

// Concrete types
class Constant : public Expression;
class Variable : public Expression;
```

**Implementation Details**:

- `Constant`: Stores a `double` value
- `Variable`: Stores a `std::string` name, looks up in variable map during evaluation
- Use `const` correctness throughout
- Virtual destructor for proper cleanup

**Testable Deliverables**:

1. ✅ `Constant(42.0)->evaluate({})` returns `42.0`
2. ✅ `Constant(-3.14)->evaluate({})` returns `-3.14`
3. ✅ `Variable("x")->evaluate({{"x", 5.0}})` returns `5.0`
4. ✅ `Variable("y")->evaluate({{"x", 1.0}, {"y", 2.0}})` returns `2.0`
5. ✅ `Variable("missing")->evaluate({{"x", 1.0}})` throws `std::out_of_range` or returns NaN (choose one)
6. ✅ Files compile without errors when added to CMakeLists.txt
7. ✅ Code compiles with `-Wall -Wextra -Werror` flags

#### 1.2 Add Arithmetic Operations ✅

**Files**: `include/expression.h`, `src/expression.cpp`

```cpp
enum class BinaryOperator { Add, Subtract, Multiply, Divide };

class BinaryOp : public Expression {
    std::shared_ptr<Expression> left;
    std::shared_ptr<Expression> right;
    BinaryOperator op;
};
```

**Implementation Details**:

- Four operators: Add, Subtract, Multiply, Divide
- Divide operation: Check for zero, return `std::numeric_limits<double>::quiet_NaN()` if zero
- Include `<cmath>` for `std::isnan()` checks
- Recursive evaluation of left/right sub-expressions

**Testable Deliverables**:

1. ✅ Addition: `BinaryOp(Constant(5), Constant(3), Add)->evaluate({})` returns `8.0`
2. ✅ Subtraction: `BinaryOp(Constant(5), Constant(3), Subtract)->evaluate({})` returns `2.0`
3. ✅ Multiplication: `BinaryOp(Constant(5), Constant(3), Multiply)->evaluate({})` returns `15.0`
4. ✅ Division: `BinaryOp(Constant(6), Constant(3), Divide)->evaluate({})` returns `2.0`
5. ✅ Divide by zero: `BinaryOp(Constant(5), Constant(0), Divide)->evaluate({})` returns NaN
6. ✅ `std::isnan(result)` returns true for divide-by-zero result
7. ✅ Nested: `(5 + 3) * 2` evaluates to `16.0`
8. ✅ With variables: `(x + y)` with `x=10, y=20` evaluates to `30.0`

#### 1.3 Test Deterministic Evaluation ✅

**Files**: `tests/test_expression.cpp`

**Test Cases**:

- Constant evaluation returns stored value
- Variable lookup from map
- Addition: `5 + 3 = 8`
- Subtraction: `5 - 3 = 2`
- Multiplication: `5 * 3 = 15`
- Division: `6 / 3 = 2`
- Divide by zero returns NaN
- Nested expressions: `(a + b) * (c - d)`
- Missing variable throws or returns NaN (decide behavior)

**Testable Deliverables**:

1. ✅ All test cases in `test_expression.cpp` pass
2. ✅ Test suite includes minimum 15 test cases (EXPECT_EQ, EXPECT_TRUE for isnan)
3. ✅ `TEST(ExpressionTest, ConstantEvaluation)` - passes
4. ✅ `TEST(ExpressionTest, VariableLookup)` - passes
5. ✅ `TEST(ExpressionTest, AddOperation)` - passes
6. ✅ `TEST(ExpressionTest, SubtractOperation)` - passes
7. ✅ `TEST(ExpressionTest, MultiplyOperation)` - passes
8. ✅ `TEST(ExpressionTest, DivideOperation)` - passes
9. ✅ `TEST(ExpressionTest, DivideByZero)` - passes
10. ✅ `TEST(ExpressionTest, NestedExpression)` - tests `(a+b)*(c-d)` - passes
11. ✅ `TEST(ExpressionTest, ComplexNested)` - tests `((x*y)+(a-b))/(c+d)` - passes
12. ✅ `ctest --test-dir build --output-on-failure` shows all tests passing
13. ✅ No memory leaks with shared_ptr (valgrind clean if available)

**Deliverable**: Expression trees evaluate correctly with fixed variable values

```cpp
auto expr = ...; // (x + y) * 2
double result = expr->evaluate({{"x", 5.0}, {"y", 3.0}}); // returns 16.0
```

---

## Phase 2: Distribution System (Stochastic Variables) ✅ COMPLETE

### Objective

Implement probability distributions and variable registry for sampling.

### Tasks

#### 2.1 Implement Distribution Abstractions ✅

**Files**: `include/distribution.h`, `src/distribution.cpp`

```cpp
class Distribution {
public:
    virtual ~Distribution() = default;
    virtual double sample(std::mt19937& rng) const = 0;
};

class NormalDistribution : public Distribution {
    double mean_;
    double stddev_;
    mutable std::normal_distribution<double> dist_;
};

class UniformDistribution : public Distribution {
    double min_;
    double max_;
    mutable std::uniform_real_distribution<double> dist_;
};
```

**Implementation Details**:

- Include `<random>` for standard library distributions
- Store distribution parameters in constructors
- `mutable` distribution objects (state changes during sampling)
- NormalDistribution constructor: `NormalDistribution(double mean, double stddev)`
- UniformDistribution constructor: `UniformDistribution(double min, double max)`

**Testable Deliverables**:

1. ✅ `NormalDistribution(0, 1)` compiles and instantiates
2. ✅ `UniformDistribution(0, 1)` compiles and instantiates
3. ✅ `dist.sample(rng)` returns a `double` value
4. ✅ Sampling normal distribution 10 times produces 10 different values (with high probability)
5. ✅ Sampling uniform distribution 10 times produces values in `[min, max]` range
6. ✅ Same seed produces identical first 5 samples
7. ✅ Files compile and link with `<random>` header included

#### 2.2 Create Variable Registry ✅

**Files**: `include/variable_registry.h`, `src/variable_registry.cpp`

```cpp
class VariableRegistry {
    std::map<std::string, std::shared_ptr<Distribution>> variables_;
    
public:
    void registerVariable(const std::string& name, 
                         std::shared_ptr<Distribution> dist);
    std::map<std::string, double> sampleAll(std::mt19937& rng) const;
    bool hasVariable(const std::string& name) const;
};
```

**Implementation Details**:

- `registerVariable()`: Add or replace variable distribution
- `sampleAll()`: Sample all registered distributions once, return name->value map
- `hasVariable()`: Check if variable is registered
- Consider adding `getVariableNames()` for introspection

**Testable Deliverables**:

1. ✅ `registry.registerVariable("x", normal_dist)` executes without error
2. ✅ `registry.hasVariable("x")` returns `true` after registration
3. ✅ `registry.hasVariable("y")` returns `false` for unregistered variable
4. ✅ `registry.sampleAll(rng)` returns `std::map<std::string, double>` with correct size
5. ✅ After registering 3 variables, `sampleAll()` returns map with 3 entries
6. ✅ `sampleAll()["x"]` returns a valid double value
7. ✅ Calling `sampleAll()` twice produces different values (probabilistically)
8. ✅ Re-registering same variable name replaces previous distribution

#### 2.3 Test Distribution Sampling ✅

**Files**: `tests/test_distribution.cpp`

**Test Cases**:

- Normal distribution: sample 10000 times, verify mean within ±3σ/√N
- Normal distribution: verify standard deviation within tolerance
- Uniform distribution: verify mean ≈ (min+max)/2
- Uniform distribution: verify all samples in [min, max]
- Deterministic seeding produces same sequence
- Registry: register and sample multiple variables
- Registry: sample returns correct number of variables

**Statistical Validation**:

```cpp
// Example test pattern
std::mt19937 rng(42); // deterministic seed
NormalDistribution dist(100.0, 15.0);
std::vector<double> samples(10000);
for (auto& s : samples) s = dist.sample(rng);

double mean = std::accumulate(...) / samples.size();
EXPECT_NEAR(mean, 100.0, 1.0); // Within reasonable tolerance
```

**Testable Deliverables**:

1. ✅ `TEST(DistributionTest, NormalMean)` - 10000 samples have mean within ±0.5 of expected
2. ✅ `TEST(DistributionTest, NormalStdDev)` - 10000 samples have stddev within ±0.5 of expected
3. ✅ `TEST(DistributionTest, UniformMean)` - 10000 samples have mean within ±0.05 of (min+max)/2
4. ✅ `TEST(DistributionTest, UniformRange)` - all 10000 samples in [min, max]
5. ✅ `TEST(DistributionTest, DeterministicSeeding)` - same seed produces same first 100 samples
6. ✅ `TEST(VariableRegistryTest, MultipleVariables)` - register 3 variables, sample all, verify 3 values
7. ✅ `TEST(VariableRegistryTest, SampleValues)` - sampled values are different on repeated calls
8. ✅ All tests in `test_distribution.cpp` pass with `ctest`
9. ✅ Test suite runs in under 5 seconds

**Deliverable**: Variables with distributions can be sampled independently

```cpp
VariableRegistry registry;
registry.registerVariable("x", std::make_shared<NormalDistribution>(0, 1));
registry.registerVariable("y", std::make_shared<UniformDistribution>(-1, 1));

std::mt19937 rng(seed);
auto samples = registry.sampleAll(rng); // {{"x", 0.234}, {"y", -0.567}}
```

---

## Phase 3: Monte Carlo Evaluation (Simulation Engine) ✅ COMPLETE

### Objective

Combine expression trees with distributions for Monte Carlo simulation.

### Tasks

#### 3.1 Build Monte Carlo Evaluator ✅

**Files**: `include/monte_carlo_evaluator.h`, `src/monte_carlo_evaluator.cpp`

```cpp
struct SimulationResult {
    std::vector<double> samples;
    double mean;
    double stddev;
    double min;
    double max;
    size_t validSampleCount;
    size_t totalSampleCount;
};

class MonteCarloEvaluator {
    size_t numSamples_;
    std::mt19937 rng_;
    
public:
    MonteCarloEvaluator(size_t numSamples, 
                       std::optional<unsigned> seed = std::nullopt);
    
    SimulationResult evaluate(std::shared_ptr<Expression> expr,
                             const VariableRegistry& registry);
};
```

**Implementation Details**:

- Constructor: Initialize RNG with seed or `std::random_device`
- `evaluate()`: Loop numSamples times, sample variables, evaluate expression
- Store all samples in result vector
- Skip NaN values when computing mean/stddev (track valid count)
- Use Welford's online algorithm for numerically stable variance computation
- Include `<numeric>`, `<algorithm>` for statistical calculations

**Testable Deliverables**:

1. ✅ `MonteCarloEvaluator(1000)` constructs successfully
2. ✅ `MonteCarloEvaluator(1000, 42)` constructs with seed=42
3. ✅ `evaluator.evaluate(expr, registry)` returns `SimulationResult`
4. ✅ `result.samples.size()` equals `numSamples` (1000)
5. ✅ `result.totalSampleCount` equals `numSamples`
6. ✅ `result.validSampleCount <= result.totalSampleCount`
7. ✅ `result.mean` is a finite double (not NaN, not Inf)
8. ✅ `result.stddev >= 0.0`
9. ✅ `result.min <= result.mean <= result.max` (for valid samples)
10. ✅ With deterministic seed, two evaluations produce identical results
11. ✅ Expression with no divide-by-zero has `validSampleCount == totalSampleCount`
12. ✅ Expression with certain divide-by-zero has `validSampleCount < totalSampleCount`

**Algorithm Outline**:

```cpp
SimulationResult evaluate(expr, registry) {
    result.samples.reserve(numSamples_);
    
    for (i = 0; i < numSamples_; ++i) {
        variables = registry.sampleAll(rng_);
        value = expr->evaluate(variables);
        result.samples.push_back(value);
    }
    
    // Compute statistics excluding NaN
    valid_samples = filter_out_nan(result.samples);
    result.validSampleCount = valid_samples.size();
    result.totalSampleCount = numSamples_;
    result.mean = compute_mean(valid_samples);
    result.stddev = compute_stddev(valid_samples, mean);
    result.min = *min_element(valid_samples);
    result.max = *max_element(valid_samples);
    
    return result;
}
```

#### 3.2 Test Simulation Accuracy ✅

**Files**: `tests/test_monte_carlo.cpp`

**Test Cases with Known Analytical Results**:

1. **Sum of two independent normals**:
   - X ~ N(μ₁, σ₁²), Y ~ N(μ₂, σ₂²)
   - X + Y ~ N(μ₁+μ₂, σ₁²+σ₂²)
   - Test: mean ≈ μ₁+μ₂, variance ≈ σ₁²+σ₂²

2. **Constant multiplication**:
   - X ~ N(μ, σ²)
   - c*X ~ N(c*μ, c²*σ²)
   - Test with c=2, c=-1, c=0

3. **Sum of uniform distributions**:
   - X ~ U(0,1), Y ~ U(0,1)
   - X + Y has mean=1, variance=1/6
   - Test convergence with different sample sizes (100, 1000, 10000)

4. **Division by zero handling**:
   - X / 0 produces NaN samples
   - Result excludes NaN from statistics
   - validSampleCount < totalSampleCount

5. **Deterministic seeding**:
   - Same seed produces identical results
   - Different seeds produce different results

**Convergence Test Example**:

```cpp
// X ~ N(5, 2), Y ~ N(3, 1)
// X + Y should have mean=8, stddev=sqrt(5)≈2.236
VariableRegistry registry;
registry.registerVariable("x", std::make_shared<NormalDistribution>(5, 2));
registry.registerVariable("y", std::make_shared<NormalDistribution>(3, 1));

auto expr = ...; // x + y
MonteCarloEvaluator evaluator(10000, 42);
auto result = evaluator.evaluate(expr, registry);

EXPECT_NEAR(result.mean, 8.0, 0.1);
EXPECT_NEAR(result.stddev, 2.236, 0.1);
```

**Testable Deliverables**:

1. ✅ `TEST(MonteCarloTest, SumOfNormals)` - X~N(5,2) + Y~N(3,1) has mean≈8.0 ±0.15
2. ✅ `TEST(MonteCarloTest, SumOfNormalsStdDev)` - variance matches σ₁²+σ₂² (stddev≈2.236 ±0.15)
3. ✅ `TEST(MonteCarloTest, ConstantMultiplication)` - 2*X where X~N(10,3) has mean≈20 and stddev≈6
4. ✅ `TEST(MonteCarloTest, NegativeMultiplication)` - (-1)*X where X~N(5,2) has mean≈-5
5. ✅ `TEST(MonteCarloTest, UniformSum)` - X~U(0,1) + Y~U(0,1) has mean≈1.0 ±0.05
6. ✅ `TEST(MonteCarloTest, DivideByZero)` - X/0 has validSampleCount=0, all samples are NaN
7. ✅ `TEST(MonteCarloTest, DivideByVariable)` - X/Y where Y~N(0,1) has validSampleCount < totalSampleCount
8. ✅ `TEST(MonteCarloTest, DeterministicSeed)` - same seed, same expression, same results
9. ✅ `TEST(MonteCarloTest, DifferentSeeds)` - different seeds produce different results
10. ✅ `TEST(MonteCarloTest, Convergence)` - error decreases with N=100, N=1000, N=10000
11. ✅ All tests in `test_monte_carlo.cpp` pass
12. ✅ Test suite completes in under 10 seconds

**Deliverable**: End-to-end Monte Carlo simulation works correctly

```cpp
auto result = evaluator.evaluate(expression, registry);
// result.mean, result.stddev, result.samples available
```

#### 3.3 Add Convergence Tracking ✅

**Files**: `include/monte_carlo_evaluator.h`, `src/monte_carlo_evaluator.cpp`, `tests/test_monte_carlo.cpp`

**Objective**: Track how mean, standard deviation, and other statistics converge as more samples are accumulated.

**Enhanced SimulationResult Structure**:

```cpp
struct ConvergencePoint {
    size_t sampleCount;        ///< Number of samples at this point
    double mean;               ///< Running mean at this point
    double stddev;             ///< Running standard deviation at this point
    size_t validCount;         ///< Valid (non-NaN) samples at this point
};

struct SimulationResult {
    std::vector<double> samples;           ///< All samples including NaN values
    double mean;                           ///< Final mean of valid samples
    double stddev;                         ///< Final standard deviation
    double min;                            ///< Minimum of valid samples
    double max;                            ///< Maximum of valid samples
    size_t validSampleCount;              ///< Number of non-NaN samples
    size_t totalSampleCount;              ///< Total number of samples
    std::vector<ConvergencePoint> convergenceHistory;  ///< Statistics at intervals
};
```

**Implementation Details**:

- Record statistics at regular intervals (e.g., every 100 samples, or logarithmic: 10, 100, 1000, etc.)
- Use existing Welford's algorithm to compute running statistics efficiently
- Option 1: Fixed interval (e.g., record every N samples)
- Option 2: Logarithmic intervals (powers of 10: 10, 100, 1000, 10000)
- Option 3: Percentage milestones (1%, 5%, 10%, 25%, 50%, 75%, 90%, 95%, 100%)
- Add optional parameter to control granularity: `evaluate(expr, registry, recordInterval = 0)`
  - `recordInterval = 0`: No convergence tracking (default, backward compatible)
  - `recordInterval = 100`: Record every 100 samples
  - `recordInterval = -1`: Use smart intervals (logarithmic or percentage-based)

**API Enhancement**:

```cpp
class MonteCarloEvaluator {
public:
    // Existing constructor
    MonteCarloEvaluator(size_t numSamples, 
                       std::optional<unsigned> seed = std::nullopt);
    
    // Enhanced evaluate with convergence tracking
    SimulationResult evaluate(std::shared_ptr<Expression> expr,
                             const VariableRegistry& registry,
                             int convergenceInterval = 0);
    
    // Helper to get smart intervals based on total samples
    std::vector<size_t> computeConvergenceIntervals(size_t totalSamples) const;
};
```

**Use Cases**:

1. **Assess simulation quality**: See if statistics have stabilized
2. **Determine optimal sample size**: Find when additional samples provide diminishing returns
3. **Visualize convergence**: Plot mean/stddev vs. sample count
4. **Early stopping**: Detect when simulation has converged sufficiently
5. **Debugging**: Identify if specific variable ranges cause convergence issues

**Testable Deliverables**:

1. ✅ `SimulationResult` contains `convergenceHistory` vector
2. ✅ With `convergenceInterval=0` (default), `convergenceHistory` is empty (backward compatible)
3. ✅ With `convergenceInterval=100`, history recorded every 100 samples
4. ✅ With `convergenceInterval=-1`, uses smart intervals (logarithmic)
5. ✅ Last entry in convergenceHistory matches final statistics
6. ✅ ConvergencePoint mean values show decreasing variance as sampleCount increases
7. ✅ For deterministic seed, convergenceHistory is reproducible
8. ✅ `TEST(MonteCarloTest, ConvergenceBackwardCompatible)` - default behavior unchanged
9. ✅ `TEST(MonteCarloTest, ConvergenceFixedInterval)` - fixed interval recording
10. ✅ `TEST(MonteCarloTest, ConvergenceSmartIntervals)` - smart intervals work
11. ✅ `TEST(MonteCarloTest, ConvergenceStandardErrorDecreases)` - SE decreases over time
12. ✅ `TEST(MonteCarloTest, ConvergenceDeterministic)` - reproducible with seed
13. ✅ `TEST(MonteCarloTest, ConvergenceWithNaN)` - handles NaN values correctly
14. ✅ `TEST(MonteCarloTest, ConvergenceIncludesFinalCount)` - final count always included
15. ✅ `TEST(MonteCarloTest, ConvergenceMeanStabilizes)` - mean stabilizes over time
16. ✅ Convergence tracking adds minimal overhead (< 5% execution time)
17. ✅ Can access intermediate statistics: `result.convergenceHistory[i].mean`

**Example Usage**:

```cpp
MonteCarloEvaluator evaluator(10000, 42);

// Enable convergence tracking with smart intervals
auto result = evaluator.evaluate(expr, registry, -1);

// Analyze convergence
std::cout << "Convergence Analysis:\n";
for (const auto& point : result.convergenceHistory) {
    std::cout << "After " << point.sampleCount << " samples: "
              << "mean = " << point.mean 
              << ", stddev = " << point.stddev << "\n";
}

// Check if simulation converged
if (result.convergenceHistory.size() >= 2) {
    auto& last = result.convergenceHistory.back();
    auto& secondLast = result.convergenceHistory[result.convergenceHistory.size() - 2];
    double meanChange = std::abs(last.mean - secondLast.mean);
    if (meanChange < 0.01) {
        std::cout << "Simulation converged!\n";
    }
}
```

**Smart Interval Strategy** (for `convergenceInterval = -1`):

- For N samples, record at: 10, 100, 1000, ..., up to N
- Or: 1%, 5%, 10%, 25%, 50%, 75%, 90%, 95%, 100%
- Ensures ~log(N) data points regardless of total samples
- Example for 10000 samples: [10, 100, 1000, 10000] or [100, 500, 1000, 2500, 5000, 7500, 9000, 9500, 10000]

**Deliverable**: Access to convergence behavior for simulation introspection

```cpp
auto result = evaluator.evaluate(expr, registry, -1);
// result.convergenceHistory shows how statistics evolved
// Useful for determining if more samples are needed
```

---

## Phase 4: Expression Builder API (Ergonomic Interface)

### Objective

Provide fluent C++ API for natural expression construction.

### Tasks

#### 4.1 Add Fluent Builder Interface

**Files**: `include/expression_builder.h`, `src/expression_builder.cpp`

```cpp
class ExpressionBuilder {
    std::shared_ptr<Expression> expr_;
    
public:
    // Constructors
    explicit ExpressionBuilder(std::shared_ptr<Expression> expr);
    
    // Operators
    ExpressionBuilder operator+(const ExpressionBuilder& other) const;
    ExpressionBuilder operator-(const ExpressionBuilder& other) const;
    ExpressionBuilder operator*(const ExpressionBuilder& other) const;
    ExpressionBuilder operator/(const ExpressionBuilder& other) const;
    
    // Factory methods
    static ExpressionBuilder constant(double value);
    static ExpressionBuilder variable(const std::string& name);
    
    // Access underlying expression
    std::shared_ptr<Expression> get() const { return expr_; }
};

// Convenience for mixed operations
ExpressionBuilder operator+(double lhs, const ExpressionBuilder& rhs);
ExpressionBuilder operator-(double lhs, const ExpressionBuilder& rhs);
ExpressionBuilder operator*(double lhs, const ExpressionBuilder& rhs);
ExpressionBuilder operator/(double lhs, const ExpressionBuilder& rhs);
```

**Implementation Details**:

- Each operator creates new `BinaryOp` expression
- Wrap constants automatically (e.g., `x + 5.0` works)
- Return new `ExpressionBuilder` wrapping result
- Copy semantics: operations don't modify operands
- Consider adding unary minus: `-x`

**Testable Deliverables**:

1. ✅ `ExpressionBuilder::constant(5.0)` creates valid builder
2. ✅ `ExpressionBuilder::variable("x")` creates valid builder
3. ✅ `x + y` compiles and creates BinaryOp with Add operator
4. ✅ `x - y` compiles and creates BinaryOp with Subtract operator
5. ✅ `x * y` compiles and creates BinaryOp with Multiply operator
6. ✅ `x / y` compiles and creates BinaryOp with Divide operator
7. ✅ `x + 5.0` compiles (mixed builder + double)
8. ✅ `5.0 + x` compiles (mixed double + builder)
9. ✅ `(x + y) * 2.0` compiles and builds correct tree
10. ✅ `builder.get()` returns `shared_ptr<Expression>`
11. ✅ Built expression can be evaluated: `(x+y).get()->evaluate({{"x",1},{"y",2}})` returns 3.0
12. ✅ Complex expression `(x*x + y*y) / 2.0` evaluates correctly

**Usage Example**:

```cpp
using namespace tt_int;

auto x = ExpressionBuilder::variable("x");
auto y = ExpressionBuilder::variable("y");

// Natural C++ syntax
auto expr = (x + y) * 2.0 - x / y;

// Evaluate with Monte Carlo
MonteCarloEvaluator evaluator(10000);
auto result = evaluator.evaluate(expr.get(), registry);
```

#### 4.2 Test Builder Syntax and Integration

**Files**: `tests/test_builder.cpp`, `tests/test_integration.cpp`

**Builder Tests** (`test_builder.cpp`):

- Constant creation
- Variable creation
- Each operator (+, -, *, /)
- Mixed operations (builder + double)
- Complex expressions: `(x + y) * (x - y)`
- Expression reuse: `auto x2 = x * x; auto expr = x2 + x2;`

**Integration Tests** (`test_integration.cpp`):

1. **Portfolio value calculation**:

   ```cpp
   // stock_price ~ N(100, 15), quantity ~ U(10, 20)
   // value = stock_price * quantity
   ```

2. **Ratio with divide-by-zero**:

   ```cpp
   // numerator ~ N(10, 2), denominator ~ N(0, 1)
   // ratio = numerator / denominator
   // Some samples will be NaN
   ```

3. **Complex formula**:

   ```cpp
   // a ~ N(5, 1), b ~ N(3, 0.5), c ~ U(1, 2)
   // result = (a + b) * c - a / b
   ```

**Testable Deliverables**:

1. ✅ `TEST(BuilderTest, ConstantCreation)` - creates and evaluates correctly
2. ✅ `TEST(BuilderTest, VariableCreation)` - creates and looks up correctly
3. ✅ `TEST(BuilderTest, AdditionOperator)` - `x + y` evaluates correctly
4. ✅ `TEST(BuilderTest, SubtractionOperator)` - `x - y` evaluates correctly
5. ✅ `TEST(BuilderTest, MultiplicationOperator)` - `x * y` evaluates correctly
6. ✅ `TEST(BuilderTest, DivisionOperator)` - `x / y` evaluates correctly
7. ✅ `TEST(BuilderTest, MixedBuilderDouble)` - `x + 5.0` evaluates correctly
8. ✅ `TEST(BuilderTest, MixedDoubleBuilder)` - `5.0 + x` evaluates correctly
9. ✅ `TEST(BuilderTest, ComplexExpression)` - `(x+y)*(x-y)` evaluates to x²-y²
10. ✅ `TEST(BuilderTest, ExpressionReuse)` - `x2 = x*x; expr = x2 + x2` works
11. ✅ `TEST(IntegrationTest, PortfolioValue)` - stock_price * quantity simulation
12. ✅ `TEST(IntegrationTest, RatioWithDivideByZero)` - handles NaN samples correctly
13. ✅ `TEST(IntegrationTest, ComplexFormula)` - multi-operation expression works
14. ✅ All builder tests pass with `ctest`
15. ✅ All integration tests pass with `ctest`
16. ✅ Portfolio test produces reasonable mean (expected ≈ 1500) and positive stddev

**Deliverable**: Clean, natural C++ API for building and evaluating expressions

```cpp
auto x = ExpressionBuilder::variable("x");
auto y = ExpressionBuilder::variable("y");
auto expr = x * x + y * y; // Builds expression tree
```

---

## Phase 5: Project Integration ✅ COMPLETE

### Objective

Integrate calculator into project build system and documentation.

### Tasks

#### 5.1 Update Build Configuration ✅

**File**: `CMakeLists.txt`

**Changes Needed**:

```cmake
# Add new source files to hello_lib
add_library(hello_lib
    src/hello.cpp
    src/expression.cpp
    src/distribution.cpp
    src/variable_registry.cpp
    src/monte_carlo_evaluator.cpp
    src/expression_builder.cpp
)

# Add test files to tests target
add_executable(tests
    tests/test_hello.cpp
    tests/test_expression.cpp
    tests/test_distribution.cpp
    tests/test_monte_carlo.cpp
    tests/test_builder.cpp
    tests/test_integration.cpp
)
```

**Testable Deliverables**:

1. ✅ `cmake -S . -B build` completes without errors
2. ✅ `cmake --build build` compiles all source files successfully
3. ✅ No compiler warnings with `-Wall -Wextra`
4. ✅ `build/hello_lib` or `build/libhello_lib.a` exists
5. ✅ `build/tt_int` executable exists and runs
6. ✅ `build/tests` executable exists and runs
7. ✅ All 6 new .cpp files are included in hello_lib target
8. ✅ All 6 test files are included in tests target

#### 5.2 Update Documentation ✅

**File**: `README.md`

**Testable Deliverables**:

1. ✅ README.md contains "Monte Carlo Expression Calculator" section
2. ✅ Quick Start code example is present and syntactically correct
3. ✅ All four operations (+, -, *, /) are documented
4. ✅ Both distribution types (Normal, Uniform) are documented with constructor signatures
5. ✅ Code examples use correct namespace (`tt_int`)
6. ✅ Example code can be copy-pasted and compiled successfully

**Add Section**:

```markdown
## Monte Carlo Expression Calculator

### Quick Start

```cpp
#include "expression_builder.h"
#include "distribution.h"
#include "variable_registry.h"
#include "monte_carlo_evaluator.h"

using namespace tt_int;

// Define variables with distributions
VariableRegistry registry;
registry.registerVariable("x", std::make_shared<NormalDistribution>(100, 15));
registry.registerVariable("y", std::make_shared<UniformDistribution>(0, 10));

// Build expression
auto x = ExpressionBuilder::variable("x");
auto y = ExpressionBuilder::variable("y");
auto expr = (x + y) / 2.0;

// Run Monte Carlo simulation
MonteCarloEvaluator evaluator(10000);
auto result = evaluator.evaluate(expr.get(), registry);

std::cout << "Mean: " << result.mean << std::endl;
std::cout << "Std Dev: " << result.stddev << std::endl;
```

### Supported Operations

- Addition: `x + y`
- Subtraction: `x - y`
- Multiplication: `x * y`
- Division: `x / y` (returns NaN for divide-by-zero)

### Distributions

- **Normal**: `NormalDistribution(mean, stddev)`
- **Uniform**: `UniformDistribution(min, max)`

```

#### 5.3 Update Copilot Instructions ✅

**File**: `.github/copilot-instructions.md`

**Testable Deliverables**:

1. ✅ File contains "Calculator API Patterns" section
2. ✅ Documents ExpressionBuilder usage pattern
3. ✅ Documents shared_ptr<Expression> pattern
4. ✅ Documents std::mt19937 usage
5. ✅ Documents NaN handling strategy
6. ✅ Documents testing patterns for statistical code

**Add Section**:
```markdown
## Calculator API Patterns

### Expression Tree Construction
- Use ExpressionBuilder for user-facing API
- Use shared_ptr<Expression> for internal tree nodes
- Follow visitor pattern for operations

### Statistical Code
- Use std::mt19937 for random number generation
- Support optional seeding for reproducibility
- Use Welford's algorithm for online variance computation

### Error Handling
- Division by zero returns quiet_NaN()
- Track valid vs. invalid samples in results
- Document NaN propagation behavior

### Testing Statistical Code
- Use deterministic seeds in tests
- Test convergence with known analytical results
- Verify statistical properties with appropriate tolerances
```

#### 5.4 Create Example/Demo ✅

**File**: `examples/calculator_demo.cpp`

**Demonstrates**:

- ✅ Variable registration (5 examples)
- ✅ Expression building with natural syntax
- ✅ Monte Carlo evaluation
- ✅ Result interpretation
- ✅ Convergence tracking
- ✅ Complex formulas and expression reuse
- ✅ Verified working - builds and runs successfully

#### 5.5 Update Main Application ✅

**File**: `src/main.cpp`

**Testable Deliverables**:

1. ✅ `./build/tt_int` runs without errors
2. ✅ Output includes calculator demonstration
3. ✅ Output shows expression being evaluated
4. ✅ Output shows distribution parameters
5. ✅ Output displays mean, stddev, min, max values
6. ✅ All displayed values are reasonable (not NaN, not Inf for this demo)
7. ✅ Program exits with return code 0

**Option 1**: Keep hello world, add calculator demo
**Option 2**: Replace with calculator showcase

```cpp
#include <iostream>
#include "hello.h"
#include "expression_builder.h"
#include "distribution.h"
#include "variable_registry.h"
#include "monte_carlo_evaluator.h"

int main() {
    // Original hello world
    std::cout << tt_int::getGreeting() << "\n\n";
    
    // Calculator demo
    std::cout << "Monte Carlo Calculator Demo\n";
    std::cout << "============================\n\n";
    
    using namespace tt_int;
    
    VariableRegistry registry;
    registry.registerVariable("x", 
        std::make_shared<NormalDistribution>(100, 15));
    registry.registerVariable("y", 
        std::make_shared<UniformDistribution>(0, 10));
    
    auto x = ExpressionBuilder::variable("x");
    auto y = ExpressionBuilder::variable("y");
    auto expr = (x + y) / 2.0;
    
    MonteCarloEvaluator evaluator(10000, 42);
    auto result = evaluator.evaluate(expr.get(), registry);
    
    std::cout << "Expression: (x + y) / 2\n";
    std::cout << "  x ~ Normal(100, 15)\n";
    std::cout << "  y ~ Uniform(0, 10)\n\n";
    std::cout << "Results from 10,000 simulations:\n";
    std::cout << "  Mean:   " << result.mean << "\n";
    std::cout << "  StdDev: " << result.stddev << "\n";
    std::cout << "  Min:    " << result.min << "\n";
    std::cout << "  Max:    " << result.max << "\n";
    
    return 0;
}
```

**Deliverable**: Fully integrated calculator with documentation and examples

---

## Development Workflow

### Per-Phase Workflow

1. **Implement** core functionality for the phase
2. **Write tests** before or alongside implementation (TDD encouraged)
3. **Run tests** and verify all pass: `ctest --test-dir build --output-on-failure`
4. **Update CMakeLists.txt** to include new files
5. **Commit** with descriptive message
6. **Review** phase deliverable is working before moving to next phase

### Testing Strategy

- **Unit tests**: Test individual classes in isolation
- **Integration tests**: Test component interactions (Phase 4+)
- **Statistical tests**: Use deterministic seeds, verify convergence
- **Edge cases**: Division by zero, empty registries, zero samples

### Git Commit Strategy

Suggested commit pattern per phase:

```
Phase 1.1: Add Expression base class and Constant/Variable types
Phase 1.2: Implement BinaryOp with four arithmetic operators
Phase 1.3: Add comprehensive expression evaluation tests
Phase 2.1: Implement Distribution base class and Normal/Uniform types
...
```

---

## File Structure Summary

### Header Files (`include/`)

```
include/
├── hello.h                      # Original (keep)
├── expression.h                 # Phase 1: Expression tree types
├── distribution.h               # Phase 2: Distribution types
├── variable_registry.h          # Phase 2: Variable management
├── monte_carlo_evaluator.h      # Phase 3: Simulation engine
└── expression_builder.h         # Phase 4: Fluent API
```

### Implementation Files (`src/`)

```
src/
├── hello.cpp                    # Original (keep)
├── main.cpp                     # Update in Phase 5
├── expression.cpp               # Phase 1
├── distribution.cpp             # Phase 2
├── variable_registry.cpp        # Phase 2
├── monte_carlo_evaluator.cpp    # Phase 3
└── expression_builder.cpp       # Phase 4
```

### Test Files (`tests/`)

```
tests/
├── test_hello.cpp               # Original (keep)
├── test_expression.cpp          # Phase 1
├── test_distribution.cpp        # Phase 2
├── test_monte_carlo.cpp         # Phase 3
├── test_builder.cpp             # Phase 4
└── test_integration.cpp         # Phase 4
```

---

## Success Criteria

### Phase 1 Complete When ✅

- ✅ Can construct expression trees programmatically
- ✅ Can evaluate expressions with fixed variable values
- ✅ All arithmetic operations work correctly
- ✅ Divide-by-zero returns NaN
- ✅ All tests pass (14 expression + 4 hello = 18 tests)

### Phase 2 Complete When ✅

- ✅ Can sample from normal and uniform distributions
- ✅ Samples have correct statistical properties
- ✅ Variable registry stores and samples multiple variables
- ✅ All tests pass with deterministic seeding (17 distribution tests)
- ✅ Total: 35 tests passing (18 from Phase 1 + 17 from Phase 2)

### Phase 3 Complete When ✅

- ✅ Monte Carlo evaluation produces correct statistics
- ✅ Results match known analytical solutions
- ✅ NaN handling works correctly
- ✅ Convergence improves with more samples
- ✅ All tests pass (15 Monte Carlo tests)
- ✅ Welford's algorithm provides numerical stability
- ✅ Optional seeding enables reproducibility

### Phase 4 Complete When ✅

- ✅ Expression builder API is intuitive and natural
- ✅ Operator overloading works correctly
- ✅ Complex expressions can be built easily
- ✅ Integration tests cover realistic scenarios
- ✅ All tests pass (26 tests: 17 builder + 9 integration)
- ✅ Main application demonstrates Phase 4 features
- ✅ Natural C++ syntax with mixed operations

### Phase 5 Complete When ✅

- ✅ All code compiles without warnings
- ✅ CMake configuration is correct
- ✅ Documentation is complete and accurate (README fully rewritten)
- ✅ Example code runs successfully (calculator_demo.cpp verified)
- ✅ All tests pass (84 tests)
- ✅ Code follows project conventions
- ✅ Copilot instructions enhanced with calculator patterns
- ✅ Standalone example with 5 comprehensive demonstrations

---

## Next Steps

**Start with Phase 1, Task 1.1**: Create `include/expression.h` and `src/expression.cpp` with the base Expression class, Constant, and Variable types.

After completing each task, run:

```bash
cmake --build build
ctest --test-dir build --output-on-failure
```

Good luck! 🚀
