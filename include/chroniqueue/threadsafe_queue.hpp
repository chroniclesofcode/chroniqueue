#include <iostream>
#include <algorithm>
#include <functional>
#include <array>
#include <stdexcept>

namespace chroniqueue {

template <class T>
class threadsafe_queue {
public:
    threadsafe_queue(int size) {
        cap = size;
        read = 0;
        write = 0;
        _full = false;
        buffer = new T[size];
    }

    ~threadsafe_queue() {
        delete[] buffer;
    }

    threadsafe_queue(const threadsafe_queue &) = delete;
    threadsafe_queue &operator=(const threadsafe_queue &&) = delete;

    /* FUNCTIONS */

    void push(T item) {
        if (full()) return;
        buffer[write] = item;

        if (++write == cap) {
            write = 0;
        }

        _full = (read == write);
    }

    T front() {
        return buffer[read];
    }

    T pop() {
        if (empty()) {
            throw std::runtime_error("Cannot pop() on empty queue.");
        }

        T ret = buffer[read];
        if (++read == cap) {
            read = 0;
        }
        
        _full = false;
        return ret;
    }

    void reset() {
        read = 0;
        write = 0;
        _full = false;
    }

    /* GETTERS */

    bool empty() {
        return !_full && read == write;
    }

    bool full() {
        return _full;
    }

    int size() {
        return (write >= read && !_full ? write - read : cap - read + write);
    }

    int capacity() {
        return cap;
    }

private:
    /* HELPER FUNCTIONS */

    /* VARIABLES */
    T* buffer;
    int cap;
    int read;
    int write;
    bool _full;
};

}