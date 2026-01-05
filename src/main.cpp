#include <iostream>
#include "hello.h"

int main() {
    std::cout << tt_int::getGreeting() << std::endl;
    std::cout << tt_int::getPersonalizedGreeting("Developer") << std::endl;
    
    int result = tt_int::add(3, 4);
    std::cout << "3 + 4 = " << result << std::endl;
    
    return 0;
}
