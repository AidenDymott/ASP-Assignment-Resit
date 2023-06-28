#include <iostream>
#include <cstdint>

#include "context.hpp"


int main() {
    //Set x to volatile int
    volatile int x = 0;

    Context c;
    //Set get_context to point at c
    get_context(&c);

    std::cout << "a message" << std::endl;
    //If statement for x = 0
    if (x == 0) {
        x = x + 1;
        //Call set_context with value of C
        set_context(&c);
    }
    return 0;
}

// clang++ context.o main_p1.cpp -o prog_1