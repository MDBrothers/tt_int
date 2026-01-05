#include "hello.h"

namespace tt_int {

std::string getGreeting() {
    return "Hello, World!";
}

std::string getPersonalizedGreeting(const std::string& name) {
    return "Hello, " + name + "!";
}

int add(int a, int b) {
    return a + b;
}

} // namespace tt_int
