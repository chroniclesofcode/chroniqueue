#pragma once 

#include <iostream>
#include <algorithm>
#include <functional>
#include <atomic>

namespace chroniqueue {

template <class T>
class spsc_queue {
public: 
    spsc_queue(int size) {
        cap = size;
    }
private:
    T* buffer;
    int cap;
    alignas(64) std::atomic<int> read(0);
    alignas(64) std::atomic<int> write(0);
};

}