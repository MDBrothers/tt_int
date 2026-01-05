#include <gtest/gtest.h>
#include "hello.h"

// Test the getGreeting function
TEST(HelloTest, GetGreeting) {
    std::string greeting = tt_int::getGreeting();
    EXPECT_EQ(greeting, "Hello, World!");
}

// Test the getPersonalizedGreeting function
TEST(HelloTest, GetPersonalizedGreeting) {
    std::string greeting = tt_int::getPersonalizedGreeting("Alice");
    EXPECT_EQ(greeting, "Hello, Alice!");
    
    greeting = tt_int::getPersonalizedGreeting("Bob");
    EXPECT_EQ(greeting, "Hello, Bob!");
}

// Test the add function
TEST(HelloTest, AddFunction) {
    EXPECT_EQ(tt_int::add(2, 3), 5);
    EXPECT_EQ(tt_int::add(-1, 1), 0);
    EXPECT_EQ(tt_int::add(0, 0), 0);
    EXPECT_EQ(tt_int::add(-5, -3), -8);
}

// Test with fixtures for more complex testing scenarios
class HelloFixture : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code that runs before each test
    }

    void TearDown() override {
        // Cleanup code that runs after each test
    }
};

TEST_F(HelloFixture, ExampleFixtureTest) {
    EXPECT_EQ(tt_int::add(10, 20), 30);
}
