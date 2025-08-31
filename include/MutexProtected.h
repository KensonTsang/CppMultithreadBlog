#pragma once
#include <mutex>

template<typename T>
class MyLocked
{
public:
    MyLocked(T& data, std::mutex& mutex) : _lock(mutex){        
        _data = &data;
    }

    MyLocked(MyLocked && r) : _lock(std::move(r._lock)) {
        _data = r._data;
        r._data = nullptr;
    }

    T* operator-> () {return _data;}
private:
    T* _data = nullptr;
    std::lock_guard<std::mutex> _lock;
};



template<typename T>
class MutexProtected {
public:
   MyLocked<T> scopedLock() { return MyLocked<T>(_data, _mtx); }

private:
    std::mutex _mtx;
    T _data;
};
