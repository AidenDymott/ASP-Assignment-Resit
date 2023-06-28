#include <iostream>
#include <cstdint>

#include "context.hpp"


int main() {
    volatile int x = 0;
    Context c;

    get_context(&c);

    std::cout << "a message" << std::endl;

    if (x == 0) {
        x = x + 1;
        set_context(&c);
    }
    return 0;
}