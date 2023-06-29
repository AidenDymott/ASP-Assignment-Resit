#include "context.hpp"

#include <iostream>
#include <cstdint>

class Fiber {
public:
//Constructor.
    Fiber(void (*func)()) : rip(func), rsp(nullptr) {}

    //Set up stack.
    void createContext() {
            //Set up stack size, dynmaically allocate the size of stack to prevent a segmentation fault.
            stack = new char[4096];
            //Cast pointer into unitpr and then back into a char*.
            rsp = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(stack) + 4096);
            //Implement the Red Zone under ptr.
            rsp = reinterpret_cast<void*>((reinterpret_cast<uintptr_t>(rsp) & -16L) - 128 - sizeof(void*));
        }


    Context getContext() {
        //Create obj context.
        Context c;
        //returns the fibers rip value.
        c.rip = reinterpret_cast<void*>(rip);
        //returns the fibers rsp value.
        c.rsp = rsp;
        return c;
    }
    
// Deconstructor.
    ~Fiber(){
        delete[] stack;
    }

private:
    //Makes rip a function pointer.
    void (*rip)();
    void* rsp;
    char* stack;
};

void foo() {
    std::cout << "you called foo" << std::endl;
    //Shouldn't be returned to main due to using a different stack.
    exit(0);
}

int main() {
    //Creates a fiber object using foo.
    Fiber f(&foo);
    //Initailises stack.
    f.createContext();

    //Gets the context from fiber class then gives control to the foo function to execute.
    Context c = f.getContext();
    set_context(&c);

    std::cout << "Back to main" << std::endl;

    return 0;
}

// clang++ fiber_main.cpp context.o -o prog