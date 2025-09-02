#pragma once
#include <mutex>

template<typename T>
class MyCondLocked {

public:
    MyCondLocked(T& data, std::mutex& mtx, std::condition_variable& condvar) : _lock(mtx) {
        _data = &data;
        _condvar = &condvar;
    }

    MyCondLocked (MyCondLocked && r) : _lock(std::move(r._lock)){
        _condvar = r._condvar;
        _data = r._data;
        r._condvar = nullptr;
        r._data = nullptr;
    }

    T* operator->() { return _data; }

    void wait() {
        if (_condvar) {
            _condvar->wait(_lock);
        }
    }

private:
    T* _data = nullptr;
    std::unique_lock<std::mutex> _lock;
    std::condition_variable* _condvar = nullptr;
};


template<typename T>
class CondVarProtected {

public:
    MyCondLocked<T> scopedLock() {return MyCondLocked<T>(_data, _mtx, _condvar);}

    void notify_all() {
        _condvar.notify_all();
    }


private:
    T _data;
    std::mutex _mtx;
    std::condition_variable _condvar;
};
