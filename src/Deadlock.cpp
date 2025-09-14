#include <iostream>
#include <thread>
#include <chrono>
#include "MutexProtected.h"


class Deadlock {

public: 
    void Example1();
    void Example2();
    void Example3();

    class Account {


    public: 

        Account (std::string name, double credit) : _name(std::move(name)), _credit(credit) {            
            std::cout << "Account [" << _name << "], credit : [" << _credit << "] created!\n";                        
        }

        void DeadlockTransfer(Account& toAccount, double credit) {

            std::cout << "start deadlock transfer from "<< _name << " to " << toAccount._name << "\n";

            std::lock_guard<std::mutex> lock1(toAccount._mtx);

            std::cout << "lock1 \n";

            std::this_thread::sleep_for(std::chrono::seconds(1));

            std::lock_guard<std::mutex> lock2(_mtx);

            std::cout << "lock2 \n";

            _credit -= credit;
            toAccount._credit += credit;

        }

        void LockByOrderTransfer(Account& toAccount, double credit) {
            
            std::cout << "start lock by Order transfer from "<< _name << " to " << toAccount._name << "\n";

            Account* first  = this;
            Account* second = &toAccount;

            if (std::less<Account*>()(second, first)) 
                std::swap(first, second);

            std::lock_guard<std::mutex> lock1(first->_mtx);
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::lock_guard<std::mutex> lock2(second->_mtx);
            std::this_thread::sleep_for(std::chrono::seconds(1));

            _credit -= credit;
            toAccount._credit += credit;
        }

        void TryLockTransfer(Account& toAccount, double credit) {

            std::cout << "start try lock transfer from "<< _name << " to " << toAccount._name << "\n";

            for (;;) {
                
                std::unique_lock<std::mutex> lock1(_mtx);

                std::this_thread::sleep_for(std::chrono::seconds(1));

                std::unique_lock<std::mutex> lock2(toAccount._mtx, std::try_to_lock);
                if(!lock2.owns_lock()) {
                    std::cout << "unable to lock!, unlock!"<< GetName() <<"\n";
                    lock1.unlock();
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    continue;                    
                }

                std::this_thread::sleep_for(std::chrono::seconds(1));
                _credit -= credit;
                toAccount._credit += credit;
                
                break;
            }

        }

        std::string GetName() const {
            return _name;
        }

        void Print() {
            std::lock_guard<std::mutex> lock(_mtx);
            std::cout << "name: "<< _name <<" credit: "<< _credit <<"\n";
        }


    public:

        std::string _name;
        std::mutex _mtx;
        double _credit;
    };

};


// 
void Deadlock::Example1() {

    Account kenson("kenson", 100);
    Account charlie("charlie", 100);
    
    std::thread t1([&]() { kenson.DeadlockTransfer(charlie, 10);});
    std::thread t2([&]() { charlie.DeadlockTransfer(kenson, 20);});

    t1.join();
    t2.join();

    kenson.Print();
    charlie.Print();


}

void Deadlock::Example2() {

    Account kenson("kenson", 100);
    Account charlie("charlie", 100);
    
    std::thread t1([&]() { kenson.LockByOrderTransfer(charlie, 10);});
    std::thread t2([&]() { charlie.LockByOrderTransfer(kenson, 20);});

    t1.join();
    t2.join();

    kenson.Print();
    charlie.Print();

}

void Deadlock::Example3(){

    Account kenson("kenson", 100);
    Account charlie("charlie", 100);
    
    std::thread t1([&]() { kenson.TryLockTransfer(charlie, 10);});
    std::thread t2([&]() { charlie.TryLockTransfer(kenson, 20);});

    t1.join();
    t2.join();

    kenson.Print();
    charlie.Print();

}

