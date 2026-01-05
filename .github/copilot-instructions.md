# GitHub Copilot Instructions for TT_INT Project

## Project Overview

This is a modern C++ project using:
- **C++ Standard**: C++17
- **Build System**: CMake 3.14+
- **Testing Framework**: Google Test (GTest)
- **Namespace**: `tt_int`

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
