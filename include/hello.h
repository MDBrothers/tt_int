#ifndef HELLO_H
#define HELLO_H

#include <string>

namespace tt_int {

/**
 * @brief Returns a greeting message
 * @return A string containing "Hello, World!"
 */
std::string getGreeting();

/**
 * @brief Returns a personalized greeting message
 * @param name The name to greet
 * @return A string containing a personalized greeting
 */
std::string getPersonalizedGreeting(const std::string& name);

/**
 * @brief Adds two integers
 * @param a First integer
 * @param b Second integer
 * @return The sum of a and b
 */
int add(int a, int b);

} // namespace tt_int

#endif // HELLO_H
