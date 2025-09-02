#include <iostream>
#include "Basic.cpp"
#include "Practical.cpp"

int main() {

    Basic example;
    // example.Example1();
    // example.Example2();

    
    Practical practical;
    // practical.Example1();        // Single Thread
    // practical.Example2();        // Multithread with equally distribute works to 4 threads
    // practical.Example3();        // Multithread with producer/worker pattern
    // practical.Example4();        // Multithread with producer/worker pattern + MutexProtected pattern
    // practical.Example5();        // Multithread with producer/worker pattern + MutexProtected pattern + CondVar pattern

    return 0;
}
