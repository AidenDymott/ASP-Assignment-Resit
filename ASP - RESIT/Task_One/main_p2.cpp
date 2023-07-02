#include <iostream>
#include <cstdint>

#include "context.hpp"

void foo() {
    std::cout << "you called foo" << std::endl;
    //Shouldn't be returned to main due to using a different stack.
    exit(0);
}

void goo() {
    std::cout << "you called goo" << std::endl;
    //Shouldn't be returned to main due to using a different stack.
    exit(0);
}

int main() {
    //Set up data stack.
    char data[4096];
    //Cast pointer into unitptr and then back into a char*.
    char* sp = reinterpret_cast<char*>(reinterpret_cast<uintptr_t>(&data) + sizeof(data));
    //Implement the Red Zone under ptr.
    sp = reinterpret_cast<char*>((reinterpret_cast<uintptr_t>(sp) & -16L) - 128 - sizeof(void*));

    //Create obj context.
    Context c;

    //Points both stack and func pointer to the context obj
    c.rip = reinterpret_cast<void*>(&foo);
    c.rsp = sp;

    //switch control to foo
    set_context(&c);
    std::cout << "Back to main" << std::endl;

    return 0;
}

// clang++ context.o main_p2.cpp -o prog_2