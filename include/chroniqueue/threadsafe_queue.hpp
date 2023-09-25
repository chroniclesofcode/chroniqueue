#include <iostream>
#include <algorithm>
#include <functional>
#include <array>
#include <stdexcept>
#include <mutex>

namespace chroniqueue {

template <class T>
class threadsafe_queue {
public:
    threadsafe_queue(int size) {
        cap = size;
        read = 0;
        write = 0;
        is_full = false;
        buffer = new T[size];
    }

    ~threadsafe_queue() {
        delete[] buffer;
    }

    threadsafe_queue(const threadsafe_queue &) = delete;
    threadsafe_queue &operator=(const threadsafe_queue &&) = delete;

    /* FUNCTIONS */

    void push(T item) {
        std::lock_guard<std::mutex> lock(_mtx);
        if (_full()) {
            return;
        }
        buffer[write] = item;

        if (++write == cap) {
            write = 0;
        }

        is_full = (read == write);
    }

    T front() {
        std::lock_guard<std::mutex> lock(_mtx);
        return buffer[read];
    }

    T pop() {
        std::lock_guard<std::mutex> lock(_mtx);
        if (_empty()) {
            throw std::runtime_error("Cannot pop() on empty queue.");
        }

        T ret = buffer[read];
        if (++read == cap) {
            read = 0;
        }
        
        is_full = false;
        return ret;
    }

    void reset() {
        std::lock_guard<std::mutex> lock(_mtx);
        read = 0;
        write = 0;
        is_full = false;
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
        std::lock_guard<std::mutex> lock(_mtx);
        return _capacity();
    }

private:
    /* HELPER FUNCTIONS / LOCK-ASSUMED-HELD FUNCTIONS */
    bool _empty() {
        return !is_full && read == write;
    }

    bool _full() {
        return is_full;
    }

    int _size() {
        return (write >= read && !is_full ? write - read : cap - read + write);
    }

    int _capacity() {
        return cap;
    }

    /* VARIABLES */
    T* buffer;
    int cap;
    int read;
    int write;
    bool is_full;
    std::mutex _mtx;
};

}