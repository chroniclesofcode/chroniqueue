# chroniqueue

Fast, lock-free SPSC queue implementation in C++. 

TO-DO:
- Check how to stop certain release mode optimizations for better time measurements.

DONE:
- Initialize threadsafe queue (size)
- Reset the threadsafe queue
- Add data to threadsafe queue
- Get the next value (pop) from threadsafe queue
- Check whether threadsafe queue is full/empty
- Test threadsafe queue using Boost
- Initialize lockfree queue (size)
- Reset the lockfree queue
- Add data to lockfree queue
- Get the next value (pop) from lockfree queue
- Check whether lockfree queue is full/empty
- Test lockfree queue actually works via multiple threads
- Test against Boost

DISCUSSION:
Note: I am running this on x86-64, so I am fairly certain we have atomic
int reads/writes (at least if properly aligned). 
Therefore, the mutexes on certain areas of the mutex
queue CAN be removed. I don't think it will really change any of my thoughts,
but technically the stats on the mutex queue can be faster by a fair amount.

The timer is also on debug mode, since on release mode, there are a large number
of optimizations that basically render the SPSC queue to take 2 milliseconds -
I'm assuming it caches values of my loop and doesn't even run them at all or
something, I'll have to investigate why the speedup is so dramatic.

To test the validity of this, we create one SPSC queue with two threads on it
at the same time. One will push integers to it, one will pop integers off it,
and sum them all together. We will see if any integers are 'lost' during the
many transactions. 

Compared against a mutex queue vs Boost/SPSC_queue. Results in /stats folder.
Our SPSC queue was faster, by a small margin, with the mutex queue being the
slowest, as expected. 