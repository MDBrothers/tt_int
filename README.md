# TT_INT C++ Project

A modern C++ project with CMake build system and Google Test integration.

## Project Structure

```
tt_int/
├── include/          # Header files
│   └── hello.h
├── src/              # Source files
│   ├── main.cpp
│   └── hello.cpp
├── tests/            # Test files
│   └── test_hello.cpp
├── .vscode/          # VS Code configuration
│   ├── settings.json
│   ├── tasks.json
│   └── launch.json
└── CMakeLists.txt    # CMake build configuration
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
# Build and run tests
cmake --build build
ctest --test-dir build --output-on-failure
```

Or run tests directly:

```bash
./build/tests
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

### Adding New Source Files

1. Add your `.cpp` files to the `src/` directory
2. Add your `.h` header files to the `include/` directory
3. Update `CMakeLists.txt` to include new source files in the library or executables

### Writing Tests

Tests use Google Test framework. Add new test files to the `tests/` directory and update `CMakeLists.txt`:

```cmake
add_executable(tests
    tests/test_hello.cpp
    tests/your_new_test.cpp  # Add your test file here
)
```

## GitHub Copilot Instructions

See [.github/copilot-instructions.md](.github/copilot-instructions.md) for project-specific Copilot guidelines.

## License

This project is provided as-is for educational and development purposes.

