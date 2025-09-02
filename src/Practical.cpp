#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm>
#include "MutexProtected.h"
#include "CondVarProtected.h"



bool isPrimeNumber(long long int n) {
    if (n <= 1) return false;
    for (long long int i = 2; i < n; ++i) {
        if (n % i == 0) return false;
    }
    return true;
}



class Practical {
public:
    void Example1();
    void Example2();
    void Example3();
    void Example4();
    void Example5();


    class MyRequest {

    public:
        MyRequest() = default;

        void addData(int value) {
            std::lock_guard<std::mutex> lock(_mtx);
            _data.push_back(value);
        }

        void printData() {
            std::lock_guard<std::mutex> lock(_mtx);

            std::sort(_data.begin(), _data.end());
            for (const auto& val : _data) {
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }

    private:
        std::mutex _mtx;
        std::vector<int> _data;
    };

    class MyThread {

    public:
        MyThread(MyRequest& request, int start, int batchSize) : _request(request), _start(start), _batchSize(batchSize), _id(0) {
            _thread = std::thread(&MyThread::ThreadFunc, this);
            _id = std::hash<std::thread::id>()(_thread.get_id());
            std::cout << "Thread " << _id << " started for range [" << _start << ", " << (_start + _batchSize - 1) << "]" << std::endl;
        }

        ~MyThread() {
            if (_thread.joinable()) {
                _thread.join();
            }
        }

        void ThreadFunc() {

            for (int i = 0 ; i < _batchSize ; ++i) {
                if (isPrimeNumber(_start + i)) {
                    std::cout << _start + i << " is prime." << std::endl;
                    _request.addData(_start + i);
                }
            }

            std::cout << "Thread " << _id << " end" << std::endl;
            
        }

        void Join() {
            if (_thread.joinable()) {
                _thread.join();
            }
        }

    private:
        MyRequest& _request;
        std::thread _thread;
        int _start;
        int _batchSize;
        std::size_t _id;
    };

    class MyProducer {
    public:
        MyProducer(long long int start, int count)
            : _start(start), _count(count), _current(start) {}

        long long int Produce() {
            std::lock_guard<std::mutex> lock(_mtx1);
            if (_current < _start + _count) {
                auto ret = _current;
                _current++;
                return ret;
            }
            return -1;                
        }

        void AddResult(long long int result) {
            std::lock_guard<std::mutex> lock(_mtx2);
            _data.push_back(result);
        }

        void Print() {
            std::lock_guard<std::mutex> lock(_mtx2);

            std::sort(_data.begin(), _data.end());
            for (const auto& val : _data) {
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }

    private:

        long long int _start;
        long long int _count;

        std::mutex _mtx1;
        long long int _current;

        std::mutex _mtx2;
        std::vector<long long int> _data;
    };

    class MyWorker {
    
    public:
        
        MyWorker(MyProducer& producer, size_t id) : _producer(producer),
                                                    _id(id), 
                                                    _thread(std::thread(&MyWorker::Process, this)){            
            std::cout << "Thread " << _id << " started " << std::endl;
        }

        ~MyWorker() {
            Join();
        }

        void Join() {
            if (_thread.joinable()) {
                _thread.join();
            }
        }
        

    private:

        void Process() {

            for(;;) {
                auto num = _producer.Produce();
                if(num == -1) {
                    break;
                }
                
                if(isPrimeNumber(num)){
                    std::cout << num << " is prime." << std::endl;
                    _producer.AddResult(num);
                }
            }

            std::cout << "Thread " << _id << " end" << std::endl;

        }

        MyProducer& _producer;
        std::thread _thread;
        std::size_t _id;
    };

    class MyProducer2 {
    public:
        MyProducer2(long long int start, int count)
            : _start(start), _count(count) {
                auto current = _current.scopedLock();
                current->data = start;
            }

        long long int Produce() {
            auto current = _current.scopedLock();
            if (current->data < _start + _count) {
                auto ret = current->data;
                current->data++;
                return ret;
            }
            return -1;                
        }

        void AddResult(long long int result) {
            auto r = _result.scopedLock();
            r->data.push_back(result);
        }

        void Print() {
            auto result = _result.scopedLock();
            std::sort(result->data.begin(),result->data.end());
            for (const auto& val : result->data) {
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }

    private:

        long long int _start;
        long long int _count;

        struct Current
        {
            long long int data;
        };
        MutexProtected<Current> _current;

        struct Result
        {
            std::vector<long long int> data;
        };
        MutexProtected<Result> _result; 
    };

    class MyWorker2 {
    
    public:
        
        MyWorker2(MyProducer2& producer, std::size_t id) : _producer(producer),
                                                            _id(id),
                                                            _thread(std::thread(&MyWorker2::Process, this)) {            
            std::cout << "Thread " << _id << " started " << std::endl;
        }

        ~MyWorker2() {
            Join();
        }

        void Join() {
            if (_thread.joinable()) {
                _thread.join();
            }
        }
        

    private:

        void Process() {

            for(;;) {
                auto num = _producer.Produce();
                if(num == -1) {
                    break;
                }
                
                if(isPrimeNumber(num)){
                    std::cout << num << " is prime." << std::endl;
                    _producer.AddResult(num);
                }
            }

            std::cout << "Thread " << _id << " end" << std::endl;

        }

        MyProducer2& _producer;
        std::thread _thread;
        std::size_t _id;
    };

    class MyProducer3 {
    public:

        MyProducer3(long long int start, int count) {
                auto r = _request.scopedLock();  
                r->start = start;                     
                r->count = count;
                r->current = start;
            }

        long long int Produce() {
            auto r = _request.scopedLock();
            if (r->current < r->start + r->count) {
                auto ret = r->current;
                r->current++;
                return ret;
            }
            return -1;                
        }

        void AddResult(long long int result) {
            auto r = _result.scopedLock();
            r->data.push_back(result);
        }

        void ThreadEnd() {
            {
                auto r = _result.scopedLock();
                r->endedThreadCount++;
            }
            _result.notify_all();
        }

        void WaitUntilEnd() {
            auto r = _result.scopedLock();
            while (r->endedThreadCount < kThreadCount) {                
                r.wait();
            }
        }

        void Print() {
            auto result = _result.scopedLock();
            std::sort(result->data.begin(),result->data.end());
            for (const auto& val : result->data) {
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }

    private:

        struct Request
        {
            long long int start = 0;
            long long int count = 0;
            long long int current = 0;
        };
        
        struct Result
        {
            std::vector<long long int> data;
            int endedThreadCount = 0;
        };

        CondVarProtected<Request> _request;
        CondVarProtected<Result> _result; 
    };

    class MyWorker3 {
    
    public:
        
        MyWorker3(MyProducer3& producer, std::size_t id) 
                : _producer(producer),
                _id(id), 
                _thread(std::thread(&MyWorker3::Process, this)) {            
            std::cout << "Thread " << _id << " started " << std::endl;
        }

        ~MyWorker3() {
            Join();
        }

        void Join() {
            if (_thread.joinable()) {
                _thread.join();
            }
        }
        

    private:

        void Process() {

            for(;;) {
                auto num = _producer.Produce();
                if(num == -1) {
                    _producer.ThreadEnd();
                    break;
                }
                
                if(isPrimeNumber(num)){
                    std::cout << num << " is prime." << std::endl;
                    _producer.AddResult(num);
                }
            }

            std::cout << "Thread " << _id << " end" << std::endl;

        }

        MyProducer3& _producer;
        std::thread _thread;
        std::size_t _id;
    };


const long long int kPrimeStart = 1000000000;
static constexpr int kThreadCount = 4;
static constexpr int kBatchSize = 10;

};


void Practical::Example1() {
    std::cout << "========== Running Practical Example 1 ==========" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    // Single-threaded prime checking
    for (int i = 0 ; i < kThreadCount * kBatchSize ; ++i) {
        if (isPrimeNumber(kPrimeStart + i)) {
            std::cout << kPrimeStart + i << " is prime." << std::endl;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Duration: " << duration.count() << " seconds" << std::endl;

    std::cout << "=======================================" << std::endl;
}

void Practical::Example2() {
    std::cout << "========== Running Practical Example 2 ==========" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    // Multi-threaded prime checking
    MyRequest request;
    MyThread threads[kThreadCount] = {
        MyThread(request, kPrimeStart, kBatchSize),
        MyThread(request, kPrimeStart + kBatchSize, kBatchSize),
        MyThread(request, kPrimeStart + 2 * kBatchSize, kBatchSize),
        MyThread(request, kPrimeStart + 3 * kBatchSize, kBatchSize)
    };

    for (auto& thread : threads) {
        thread.Join();
    }

    request.printData();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Duration: " << duration.count() << " seconds" << std::endl;

    std::cout << "=======================================" << std::endl;
}

void Practical::Example3() {
    std::cout << "========== Running Practical Example 3 ==========" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    MyProducer producer(kPrimeStart,kBatchSize * kThreadCount);

    MyWorker workers[kThreadCount] {
        MyWorker(producer, 1),
        MyWorker(producer, 2),
        MyWorker(producer, 3),
        MyWorker(producer, 4)
    };

    for (auto& worker : workers) {
        worker.Join();
    }

    producer.Print();


    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Duration: " << duration.count() << " seconds" << std::endl;

    std::cout << "=======================================" << std::endl;
}

void Practical::Example4() {
    std::cout << "========== Running Practical Example 4 ==========" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    MyProducer2 producer(kPrimeStart,kBatchSize * kThreadCount);

    MyWorker2 workers[kThreadCount] {
        MyWorker2(producer, 1),
        MyWorker2(producer, 2),
        MyWorker2(producer, 3),
        MyWorker2(producer, 4)
    };

    for (auto& worker : workers) {
        worker.Join();
    }

    producer.Print();


    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Duration: " << duration.count() << " seconds" << std::endl;

    std::cout << "=======================================" << std::endl;

}

void Practical::Example5() {
    std::cout << "========== Running Practical Example 5 ==========" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    MyProducer3 producer(kPrimeStart,kBatchSize * kThreadCount);

    MyWorker3 workers[kThreadCount] {
        MyWorker3(producer, 1),
        MyWorker3(producer, 2),
        MyWorker3(producer, 3),
        MyWorker3(producer, 4)
    };

    producer.WaitUntilEnd();

    for (auto& worker : workers) {
        worker.Join();
    }
   

    producer.Print();


    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Duration: " << duration.count() << " seconds" << std::endl;



    std::cout << "=======================================" << std::endl;
}