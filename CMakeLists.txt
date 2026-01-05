cmake_minimum_required(VERSION 3.14)
project(tt_int VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Library
add_library(hello_lib
    src/hello.cpp
    src/expression.cpp
    src/distribution.cpp
    src/variable_registry.cpp
)

# Main executable
add_executable(tt_int
    src/main.cpp
)

target_link_libraries(tt_int hello_lib)

# Enable testing
enable_testing()

# Fetch Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Test executable
add_executable(tests
    tests/test_expression.cpp
    tests/test_hello.cpp
    tests/test_distribution.cpp
)

target_link_libraries(tests
    hello_lib
    GTest::gtest_main
)

# Register tests with CTest
include(GoogleTest)
gtest_discover_tests(tests)
