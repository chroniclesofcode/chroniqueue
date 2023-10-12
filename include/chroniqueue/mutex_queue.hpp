#pragma once

#include <iostream>
#include <algorithm>
#include <functional>
#include <array>
#include <stdexcept>
#include <mutex>

namespace chroniqueue {

template <class T>
class mutex_queue {
public:
    mutex_queue(int size) {
        cap = size+1;
        read = 0;
        write = 0;
        buffer = new T[size+1];
    }

    ~mutex_queue() {
        delete[] buffer;
    }

    mutex_queue(const mutex_queue &) = delete;
    mutex_queue &operator=(const mutex_queue &&) = delete;

    /* FUNCTIONS */

    bool push(const T &item) {
        std::lock_guard<std::mutex> lock(_mtx);
        if (_full()) {
            return false;
        }
        buffer[write] = item;

        if (++write == cap) {
            write = 0;
        }
        return true;
    }

    T front() {
        std::lock_guard<std::mutex> lock(_mtx);
        return buffer[read];
    }

    bool pop(T &item) {
        std::lock_guard<std::mutex> lock(_mtx);
        if (_empty()) {
            return false;
        }
        item = buffer[read];
        if (++read == cap) {
            read = 0;
        }
        return true;
    }

    void reset() {
        std::lock_guard<std::mutex> lock(_mtx);
        read = 0;
        write = 0;
    }

    /* GETTERS */

    bool empty() {
        std::lock_guard<std::mutex> lock(_mtx);
        return _empty();
    }

    bool full() {
        std::lock_guard<std::mutex> lock(_mtx);
        return _full();
    }

    int size() {
        std::lock_guard<std::mutex> lock(_mtx);
        return _size();
    }

    int capacity() {
        return cap;
    }

private:
    /* HELPER FUNCTIONS / LOCK-ASSUMED-HELD FUNCTIONS */
    bool _empty() {
        return read == write;
    }

    bool _full() {
        int nextWrite = write + 1 == cap ? 0 : write + 1;
        return nextWrite == read;
    }

    int _size() {
        return (write >= read && !_full() ? write - read : cap - read + write);
    }

    /* VARIABLES */
    T* buffer;
    int cap;
    int read;
    int write;
    std::mutex _mtx;
};

}