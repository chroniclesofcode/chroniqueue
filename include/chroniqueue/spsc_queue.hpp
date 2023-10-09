#pragma once 

#include <iostream>
#include <algorithm>
#include <functional>
#include <atomic>

namespace chroniqueue {

template <class T>
class spsc_queue {
public: 
    spsc_queue(int size): cap{size+1}, read{0}, write{0} {
        buffer = new T[size+1];
    }

    ~spsc_queue() {
        delete[] buffer;
    }

    bool push(T item) {
        int curr = write.load(std::memory_order_relaxed);
        int next = curr + 1 == cap ? 0 : curr + 1;
        if (next == read.load(std::memory_order_relaxed)) {
            return false;
        }
        buffer[curr] = item;
    }

    T front() {
    }

    T pop() {
    }

    void reset() {
    }

    bool empty() {

    }

    bool full() {
    }

    int size() {
    }

    int capacity() {
        return cap-1;
    }

private:
    T* buffer;
    int cap;
    alignas(64) std::atomic<int> read;
    alignas(64) std::atomic<int> write;
};

}