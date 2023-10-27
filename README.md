# chroniqueue

Fast, lock-free SPSC queue implementation in C++. 

TO-DO:

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
- Benchmark with multithreading
- Investigate different acquire/release orderings...
- Optimized cache coherency process

DISCUSSION:
Note: I am running this on x86-64, so I am fairly certain we have atomic
int reads/writes (at least if properly aligned). 
Therefore, the mutexes on certain areas of the mutex
queue CAN be removed. I don't think it will really change any of my thoughts,
but technically the stats on the mutex queue can be faster by a fair amount.

To test the correctness of this, we create one SPSC queue with two threads on it
at the same time. One will push integers to it, one will pop integers off it,
and sum them all together. We will see if any integers are 'lost' during the
many transactions. 

Compared against a mutex queue vs Boost/SPSC_queue. Results in /stats folder.
Our SPSC queue was faster, by a small margin, with the mutex queue being the
slowest, as expected. This is for singlethreaded performance.

For multithreaded performance, at first, Boost was significantly faster with
60ms vs 192ms for the same test. Now onto trying to improve faster than boost:

An issue I had: I mixed up some of the acquire/release orderings, resulting in
different speeds for my SPSC queue. It doesn't seem to affect the correctness of
my code, but it probably detected that my queue was full sometimes when it's not
really full, resulting in extra loops. The case in which this occurs is: when
I am checking if write == read for push, I used relaxed ordering for read, so
read could have been read before, when the queue was still full. In the meantime,
it could have already been incremented to become write-able, but it still returned
the old value. I added an acquire ordering on read, and it works. A similar 
situation happened with pop, check a very old commit to see it in detail.

There was also a couple issues with my original implementation with just a 
read variable and a write variable. The read and write variables are jumping from
exclusive -> shared -> modified very often, flushing the modified buffer very
often and resulting in slower performance. It's not exactly false sharing, but
suffers from both threads modifying variables in each other's caches.

After doing some research, the way I have chosen to solve this issue is to keep
a local read_local and write_local variable, which is only modified by the
thread using it (won't experience cache coherency issues...). Effectively, it
lets the readers/writers see an older version of the queue until it is full or empty.

This improved the runtime of my multithread test from 190ms average to ~23 second average, as
compared to the ~60ms runtime of boost. 

To be honest, I don't really know if I did anything wrong - such an improvement
is incredible. This shows that cache coherency issues and cache alignment processes
take an enormous amount of time/throughput. It is very important to optimize these
aspects when designing for optimal performance. This small change was only a few 
lines but made all the difference.

