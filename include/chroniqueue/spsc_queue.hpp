#pragma once 

#include <iostream>
#include <algorithm>
#include <functional>
#include <atomic>

namespace chroniqueue {

template <class T>
class spsc_queue {
public: 
    spsc_queue(int size): cap{size}, read{0}, write{0} {
    }
private:
    T* buffer;
    int cap;
    alignas(64) std::atomic<int> read;
    alignas(64) std::atomic<int> write;
};

}