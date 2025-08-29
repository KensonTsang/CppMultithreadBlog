#include <iostream>
#include <thread>
#include <chrono>

class Basic {
    
public:
    void Example1();
    void Example2();


private:

    void PrintChar1(const char c);
    void PrintChar2(const char c);
    void NoLockSumThread();
    void LockSumThread1();
    void LockSumThread2();

    const int kCharCount = 1000;
    const int kSumCount = 1000000000;


    // Example 1
    std::mutex ex1_mtx;

    // Example 2
    std::mutex ex2_mtx;
    int ex2_sum_1 = 0;
    int ex2_sum_2 = 0;

};

void Basic::PrintChar1(const char c) {
    for (size_t i = 0; i < kCharCount; i++) {
        std::cout << c;
    }
}

void Basic::PrintChar2(const char c) {    
    for (size_t i = 0; i <  kCharCount; i++) {        
        ex1_mtx.lock();
        std::cout << c;        
        ex1_mtx.unlock();
    }
}

void Basic::NoLockSumThread() {
    for (int i = 0; i < kSumCount / 2; ++i) {
        ex2_sum_1++;
    }
}

void Basic::LockSumThread1() {    
    for (int i = 0; i < kSumCount / 2; ++i) {        
        std::lock_guard<std::mutex> lock(ex2_mtx);
        ex2_sum_2++;
    }
}

void Basic::LockSumThread2() {
    int local_sum = 0;
    for (int i = 0; i < kSumCount / 2; ++i) {                
        local_sum++;
    }
    std::lock_guard<std::mutex> lock(ex2_mtx);
    ex2_sum_2 += local_sum;
}


void Basic::Example1() {
    std::cout << "========== Running Example 1 ==========" << std::endl;
    std::cout << "[NO LOCK]" << std::endl;
    std::thread t1(&Basic::PrintChar1, this, '+');
    std::thread t2(&Basic::PrintChar1, this, '-');
    t1.join();
    t2.join();
    std::cout << std::endl;
    std::cout << "---------------------------------------" << std::endl;
    std::cout << "[WITH LOCK]" << std::endl;
    std::thread t3(&Basic::PrintChar2, this, '+');
    std::thread t4(&Basic::PrintChar2, this, '-');
    t3.join();
    t4.join();
    std::cout << "=======================================" << std::endl;
}


void Basic::Example2() {
    std::cout << "========== Running Example 2 ==========" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    std::thread t1(&Basic::NoLockSumThread, this);
    std::thread t2(&Basic::NoLockSumThread, this);

    t1.join();
    t2.join();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "No lock sum from 0 to " << kSumCount << ": " << ex2_sum_1 << std::endl;
    std::cout << "Duration: " << duration.count() << " seconds" << std::endl;
    ex2_sum_2 = 0;

    std::cout << "---------------------------------------" << std::endl;
    
    start = std::chrono::high_resolution_clock::now();
    std::thread t3(&Basic::LockSumThread1, this);
    std::thread t4(&Basic::LockSumThread1, this);

    t3.join();
    t4.join();

    end = std::chrono::high_resolution_clock::now();
    duration = end - start;

    std::cout << "Lock sum from 0 to " << kSumCount << ": " << ex2_sum_2 << std::endl;
    std::cout << "Duration: " << duration.count() << " seconds" << std::endl;
    ex2_sum_2 = 0;

    std::cout << "---------------------------------------" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    std::thread t5(&Basic::LockSumThread2, this);
    std::thread t6(&Basic::LockSumThread2, this);

    t5.join();
    t6.join();

    end = std::chrono::high_resolution_clock::now();
    duration = end - start;

    std::cout << "Lock sum from 0 to " << kSumCount << ": " << ex2_sum_2 << std::endl;
    std::cout << "Duration: " << duration.count() << " seconds" << std::endl;
    ex2_sum_2 = 0;

    std::cout << "=======================================" << std::endl;
}
