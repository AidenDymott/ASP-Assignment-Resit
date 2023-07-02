#include "context.hpp"

#include <iostream>
#include <cstdint>
#include <cstring>
#include <deque>

//Ptr used by fibers, default value of null if no ptr passed through it.
void* shared_data_ptr = nullptr;

//Used to return shared ptr value.
void* get_data(){
        return shared_data_ptr;
    }

class Fiber {
public:
//Constructor.
    Fiber(void (*func)(),void* data) : rip(func), rsp(nullptr), data_ptr(data) {}


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

    void execute(){
        //Allows rip to be used outside of fiber class.
        rip();
    }
    
// Deconstructor.
    ~Fiber(){
 
    }

private:
    //Makes rip a function pointer.
    void (*rip)();
    void* rsp;
    char* stack;
    void* data_ptr;
};

class Scheduler {
public:
//Adds a new fiber to the back of the que.
    void spawn(Fiber* f) {
        fibers_.push_back(f);
    }
//Executes fibers.
    void do_it() {
        context_ = getCurrentContext();

        while (!fibers_.empty()) {
            Fiber* currentFiber = fibers_.front();
            fibers_.pop_front();

            Context c = currentFiber->getContext();
            setContext(&c);

            //Call the fiber function for execution.
            currentFiber->execute();

            delete currentFiber; //Delete fiber when done.
        }

        setContext(&context_);
    }

private:
    //Declares a member variable as a double ended que.
    std::deque<Fiber*> fibers_;
    //Context obj.
    Context context_;

    Context getCurrentContext() {
        //Context obj.
        Context c;
        //Passes addrress of obj to get_context.
        get_context(&c);
        return c;
    }
    //Pass c into set_context.
    void setContext(Context* c) {
        set_context(c);
    }
};

Scheduler s; //Global obj of the scheduler set to s.

void fiber1(){
    int* dp = static_cast<int*>(get_data());//Create an int variable of dp(data ptr) and set it to the current value of the shared ptr. (Should be 10).
    std::cout << "Fiber 1: "<< *dp << std::endl;
    *dp = *dp + 1; //Increase value of data ptr by 1.
}

void fiber2(){
    int* dp = static_cast<int*>(get_data());//Create an int variable of dp(data ptr) and set it to the current value of the shared ptr. (Should be 11).
    std::cout << "Fiber 2: "<< *dp << std::endl;
}

int main() {
    int d = 10; //Default data value.
    shared_data_ptr = &d; //Set the vaule of the shared_data_ptr to the mem address of d.

    //Create new fiber objs.
    Fiber* f1 = new Fiber(&fiber1, get_data());
    Fiber* f2 = new Fiber(&fiber2, get_data());

    //Creates a context for new objs and sets stack pointer.
    f1->createContext();
    f2->createContext();

    //Add the fibers into the scheduler.
    s.spawn(f1);
    s.spawn(f2);

    //Executes fibers in order of how they were added to the que.
    s.do_it();

    return 0;
}

// clang++ scheduler_main.cpp context.o -o prog