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
        _mtx.lock();
        if (full()) {
            _mtx.unlock();
            return;
        }
        buffer[write] = item;

        if (++write == cap) {
            write = 0;
        }

        _full = (read == write);
        _mtx.unlock();
    }

    T front() {
        _mtx.lock();
        T ret = buffer[read];
        _mtx.unlock();
        return ret;
    }

    T pop() {
        _mtx.lock();
        if (empty()) {
            _mtx.unlock();
            throw std::runtime_error("Cannot pop() on empty queue.");
        }

        T ret = buffer[read];
        if (++read == cap) {
            read = 0;
        }
        
        _full = false;
        _mtx.unlock();
        return ret;
    }

    void reset() {
        _mtx.lock();
        read = 0;
        write = 0;
        _full = false;
        _mtx.unlock();
    }

    /* GETTERS */

    bool empty() {
        _mtx.lock();
        bool ret = !_full && read == write;
        _mtx.unlock();
        return ret;
    }

    bool full() {
        _mtx.lock();
        bool ret = _full;
        _mtx.unlock();
        return ret;
    }

    int size() {
        _mtx.lock();
        int ret = (write >= read && !_full ? write - read : cap - read + write);
        _mtx.unlock();
        return ret;
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
    std::mutex _mtx;
};

}