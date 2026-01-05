# Monte Carlo Expression Calculator

A C++ expression tree calculator with statistical distribution support and Monte Carlo simulation engine. Build complex mathematical expressions with natural operator overloading and evaluate them using probability distributions.

## Features

- 🎲 **Monte Carlo Simulation**: Run thousands of simulations with stochastic variables
- 📊 **Probability Distributions**: Normal and Uniform distributions using C++ standard library
- 🌳 **Expression Trees**: Build complex mathematical expressions programmatically
- ➕ **Natural Syntax**: Use C++ operator overloading for intuitive expression building
- 📈 **Statistical Analysis**: Compute mean, standard deviation, min, max, and convergence metrics
- 🔍 **Convergence Tracking**: Monitor how statistics stabilize as samples accumulate
- ✅ **84 Tests**: Comprehensive test suite with Google Test

## Quick Start

```cpp
#include "expression_builder.h"
#include "distribution.h"
#include "variable_registry.h"
#include "monte_carlo_evaluator.h"

using namespace tt_int;

int main() {
    // Define variables with probability distributions
    VariableRegistry registry;
    registry.registerVariable("stock_price", 
        std::make_shared<NormalDistribution>(100.0, 15.0));
    registry.registerVariable("quantity", 
        std::make_shared<UniformDistribution>(10.0, 20.0));
    
    // Build expression using natural C++ syntax
    auto price = ExpressionBuilder::variable("stock_price");
    auto qty = ExpressionBuilder::variable("quantity");
    auto portfolioValue = price * qty;
    
    // Run Monte Carlo simulation
    MonteCarloEvaluator evaluator(10000, 42);  // 10k samples, seed=42
    auto result = evaluator.evaluate(portfolioValue.get(), registry);
    
    std::cout << "Mean Portfolio Value: $" << result.mean << "\n";
    std::cout << "Standard Deviation:   $" << result.stddev << "\n";
    std::cout << "Range: $" << result.min << " - $" << result.max << "\n";
    
    return 0;
}
```

## Supported Operations

| Operation | Syntax | Example |
|-----------|--------|---------|
| Addition | `x + y` | `auto sum = x + y;` |
| Subtraction | `x - y` | `auto diff = x - y;` |
| Multiplication | `x * y` | `auto product = x * y;` |
| Division | `x / y` | `auto ratio = x / y;` |
| Mixed | `x + 5.0` | `auto result = (x + 2.0) * y;` |

**Note**: Division by zero returns `NaN` and is tracked separately in simulation results.

## Probability Distributions

### Normal Distribution

```cpp
auto dist = std::make_shared<NormalDistribution>(mean, stddev);
registry.registerVariable("x", dist);
```

**Example**: Stock prices, measurement errors, IQ scores

- `NormalDistribution(100.0, 15.0)` - mean=100, standard deviation=15

### Uniform Distribution

```cpp
auto dist = std::make_shared<UniformDistribution>(min, max);
registry.registerVariable("y", dist);
```

**Example**: Random selection, dice rolls, uniform sampling

- `UniformDistribution(0.0, 10.0)` - uniform between 0 and 10

## Advanced Features

### Convergence Tracking

Monitor how simulation statistics stabilize:

```cpp
MonteCarloEvaluator evaluator(10000, 42);

// Enable smart interval tracking (-1 uses logarithmic + percentage intervals)
auto result = evaluator.evaluate(expr.get(), registry, -1);

// Analyze convergence
for (const auto& point : result.convergenceHistory) {
    std::cout << "After " << point.sampleCount << " samples: "
              << "mean = " << point.mean 
              << ", SE = " << (point.stddev / sqrt(point.sampleCount)) << "\n";
}
```

### Expression Reuse

Build sub-expressions and compose them:

```cpp
auto x = ExpressionBuilder::variable("x");
auto y = ExpressionBuilder::variable("y");

auto x_squared = x * x;
auto y_squared = y * y;
auto distance = x_squared + y_squared;  // x² + y²
```

### Complex Formulas

Combine multiple operations naturally:

```cpp
// Sharpe ratio: (return - risk_free_rate) / volatility
auto returns = ExpressionBuilder::variable("expected_return");
auto riskFree = ExpressionBuilder::constant(0.02);
auto vol = ExpressionBuilder::variable("volatility");

auto sharpeRatio = (returns - riskFree) / vol;
```

## Project Structure

```
tt_int/
├── include/                  # Header files
│   ├── expression.h         # Expression tree types (Constant, Variable, BinaryOp)
│   ├── expression_builder.h # Fluent API with operator overloading
│   ├── distribution.h       # Normal and Uniform distributions
│   ├── variable_registry.h  # Variable management and sampling
│   └── monte_carlo_evaluator.h  # Simulation engine
├── src/                     # Implementation files
│   ├── main.cpp            # Demo application
│   ├── expression.cpp
│   ├── expression_builder.cpp
│   ├── distribution.cpp
│   ├── variable_registry.cpp
│   └── monte_carlo_evaluator.cpp
├── tests/                   # Test suite (84 tests)
│   ├── test_expression.cpp      # Expression tree tests (14)
│   ├── test_distribution.cpp    # Distribution tests (17)
│   ├── test_monte_carlo.cpp     # Simulation tests (23)
│   ├── test_builder.cpp         # Builder API tests (17)
│   └── test_integration.cpp     # Integration tests (9)
├── examples/                # Standalone examples
│   └── calculator_demo.cpp
├── .github/
│   └── copilot-instructions.md  # AI assistant guidelines
└── CMakeLists.txt          # CMake build configuration
```

## Requirements

- CMake 3.14 or higher
- C++17 compatible compiler (GCC, Clang, or MSVC)
- Git

## Building the Project

### Configure and Build

```bash
# Configure the project
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

# Build the project
cmake --build build
```

### Run the Application

```bash
./build/tt_int
```

## Running Tests

```bash
# Build and run all 84 tests
cmake --build build
ctest --test-dir build --output-on-failure
```

Or run tests directly:

```bash
./build/tests
```

### Test Coverage

- **14 Expression Tests**: Constant, Variable, BinaryOp operations
- **17 Distribution Tests**: Normal, Uniform sampling and statistics
- **23 Monte Carlo Tests**: Simulation accuracy, convergence, NaN handling
- **17 Builder Tests**: Operator overloading, mixed operations, composition
- **9 Integration Tests**: Real-world scenarios (portfolio, ROI, Sharpe ratio)

## Examples

Run the demo application to see all features:

```bash
./build/tt_int
```

Or check out the standalone example:

```bash
# Build example
g++ -std=c++17 -I include examples/calculator_demo.cpp \
    src/expression.cpp src/expression_builder.cpp \
    src/distribution.cpp src/variable_registry.cpp \
    src/monte_carlo_evaluator.cpp -o calculator_demo

# Run
./calculator_demo
```

## VS Code Integration

The project includes VS Code configuration for:

- **Build Tasks**: Press `Ctrl+Shift+B` to build
- **Debugging**: Press `F5` to debug the application or tests
- **CMake Integration**: Automatic configuration on project open

### Available Tasks

- `CMake: Configure` - Configure the CMake project
- `CMake: Build` - Build the project (default build task)
- `CMake: Clean` - Clean build artifacts
- `Run Tests` - Build and run all tests
- `Run Application` - Build and run the main application

## Development

### Adding New Variables

```cpp
VariableRegistry registry;
registry.registerVariable("name", std::make_shared<DistributionType>(params));
```

### Building Expressions

Two approaches available:

**1. Direct Tree Construction** (low-level):

```cpp
auto x = std::make_shared<Variable>("x");
auto two = std::make_shared<Constant>(2.0);
auto expr = std::make_shared<BinaryOp>(x, two, BinaryOperator::Multiply);
```

**2. Builder API** (recommended):

```cpp
auto x = ExpressionBuilder::variable("x");
auto expr = x * 2.0;
```

### Deterministic vs Random Seeding

```cpp
// Deterministic (for testing)
MonteCarloEvaluator evaluator(10000, 42);  // seed=42

// Random (for production)
MonteCarloEvaluator evaluator(10000);  // uses std::random_device
```

### Adding New Test Files

Tests use Google Test framework. Add new test files to `tests/` directory and update `CMakeLists.txt`:

```cmake
add_executable(tests
    tests/test_expression.cpp
    tests/test_distribution.cpp
    tests/test_monte_carlo.cpp
    tests/test_builder.cpp
    tests/test_integration.cpp
    tests/your_new_test.cpp  # Add here
)
```

## API Reference

### SimulationResult

```cpp
struct SimulationResult {
    std::vector<double> samples;              // All samples (including NaN)
    double mean;                               // Mean of valid samples
    double stddev;                             // Standard deviation
    double min, max;                           // Range of valid samples
    size_t validSampleCount;                   // Non-NaN count
    size_t totalSampleCount;                   // Total samples
    std::vector<ConvergencePoint> convergenceHistory;  // Optional tracking
};
```

### ConvergencePoint

```cpp
struct ConvergencePoint {
    size_t sampleCount;  // Number of samples at this checkpoint
    double mean;         // Running mean
    double stddev;       // Running standard deviation
    size_t validCount;   // Valid samples so far
};
```

## Performance Notes

- **Welford's Algorithm**: Numerically stable variance computation
- **Smart Intervals**: Logarithmic checkpoints for efficient convergence tracking
- **Minimal Overhead**: Convergence tracking adds < 5% execution time
- **Memory Efficient**: Samples can be processed incrementally (though currently stored)

## Design Decisions

- **NaN Handling**: Division by zero returns `quiet_NaN()` rather than throwing exceptions
- **Shared Pointers**: Expression trees use `shared_ptr` for natural sub-expression reuse
- **No Parallelization**: Single-threaded for simplicity (can be added later)
- **Four Operations Only**: +, -, *, / (no transcendental functions like sin/cos/exp)

## Contributing

See [DEVELOPMENT_PLAN.md](DEVELOPMENT_PLAN.md) for detailed phase-by-phase development guide.

## GitHub Copilot Instructions

See [.github/copilot-instructions.md](.github/copilot-instructions.md) for project-specific Copilot guidelines.

## License

This project is provided as-is for educational and development purposes.
