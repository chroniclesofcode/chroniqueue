#pragma once 

#include <iostream>
#include <algorithm>
#include <functional>
#include <atomic>

namespace chroniqueue {

template <class T>
class spsc_queue {
public: 
    spsc_queue(int size): cap{size+1}, read{0}, read_local{0}, write{0}, write_local{0} {
        buffer = new T[size+1];
    }

    ~spsc_queue() {
        delete[] buffer;
    }

    bool push(const T &item) {
        int curr = write.load(std::memory_order_relaxed);
        int next = curr + 1 == cap ? 0 : curr + 1;
        if (next == read_local) {
            read_local = read.load(std::memory_order_acquire);
            if (next == read_local) return false;
        }
        buffer[curr] = item;
        write.store(next, std::memory_order_release);
        return true;
    }

    T front() {
        return buffer[read.load(std::memory_order_relaxed)];
    }

    bool pop(T &item) {
        int r = read.load(std::memory_order_relaxed);
        if (r == write_local) {
            write_local = write.load(std::memory_order_acquire);
            if (r == write_local) return false;
        }
        item = buffer[r];
        int next = r + 1 == cap ? 0 : r + 1;
        read.store(next, std::memory_order_release);
        return true;
    }

    void reset() {
        read.store(0, std::memory_order_relaxed);
        write.store(0, std::memory_order_relaxed);
    }

    bool empty() {
        return read.load(std::memory_order_relaxed) ==
                write.load(std::memory_order_relaxed);
    }

    bool full() {
        int curr = write.load(std::memory_order_relaxed);
        int next = curr + 1 == cap ? 0 : curr + 1;
        return next == read.load(std::memory_order_relaxed);
    }

    int size() {
        int w = write.load(std::memory_order_relaxed);
        int r = read.load(std::memory_order_relaxed);
        return (w >= r && !full() ? w - r : cap - r + w);
    }

    int capacity() {
        return cap-1;
    }

private:
    T* buffer;
    int cap;
    alignas(64) std::atomic<int> read;
    alignas(64) int read_local;
    alignas(64) std::atomic<int> write;
    alignas(64) int write_local;
};

}