#define BOOST_TEST_MODULE ALL TESTS
#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_ALTERNATIVE_INIT_API
#include <boost/test/included/unit_test.hpp>
#include <boost/lockfree/spsc_queue.hpp>

#include <iostream>
#include <iterator>
#include <algorithm>
#include <thread>
#include <functional>
#include <vector>
#include <random>

#include "chroniqueue/mutex_queue.hpp"
#include "chroniqueue/spsc_queue.hpp"
#include "chroniqueue/Timer.h"

#define STATS_ON 1
#define MULTITHREAD_TEST 0

BOOST_AUTO_TEST_CASE(mtx_integration_test) {
    chroniqueue::mutex_queue<int> q(5);
    // Testing is_empty and popping
    int tmp = 0;
    BOOST_TEST(q.empty());
    BOOST_TEST(q.size() == 0);
    q.push(100);
    BOOST_TEST(q.size() == 1);
    BOOST_TEST(!q.empty());
    q.pop(tmp);
    BOOST_TEST(q.empty());
    BOOST_TEST(q.size() == 0);

    // Checking full and if front works properly
    BOOST_TEST(!q.full());
    q.push(25);
    BOOST_TEST(q.front() == 25);
    BOOST_TEST(!q.full());
    q.push(28);
    BOOST_TEST(q.front() == 25);
    BOOST_TEST(!q.full());
    q.push(30);
    BOOST_TEST(!q.full());
    q.push(32);
    BOOST_TEST(!q.full());
    q.push(35);
    BOOST_TEST(q.full());
    int p1 = q.front();
    q.pop(tmp);
    BOOST_TEST(p1 == 25);
    int p2 = q.front();
    q.pop(tmp);
    BOOST_TEST(p2 == 28);
    int p3 = q.front();
    q.pop(tmp);
    BOOST_TEST(p3 == 30);
    BOOST_TEST(q.size() == 2);
    BOOST_TEST(!q.empty());
    
    // Checking the wrap-around write
    q.push(70);
    BOOST_TEST(q.size() == 3);
    q.push(71);
    BOOST_TEST(q.size() == 4);
    q.push(72);
    BOOST_TEST(q.size() == 5);
    int p4 = q.front();
    q.pop(tmp);
    int p5 = q.front();
    q.pop(tmp);
    int p6 = q.front();
    q.pop(tmp);
    BOOST_TEST(p4 == 32);
    BOOST_TEST(p5 == 35);
    BOOST_TEST(p6 == 70);

    // Checking pushing a LOT of times.
    q.push(1);
    BOOST_TEST(q.size() == 3);
    q.push(2);
    BOOST_TEST(q.size() == 4);
    q.push(3);
    BOOST_TEST(q.size() == 5);
    q.push(4); // Shouldn't run
    q.push(5); // Shouldn't run
    q.push(6); // Shouldn't run
    q.pop(tmp);
    BOOST_TEST(tmp == 71);
    q.pop(tmp);
    BOOST_TEST(tmp == 72);
    q.pop(tmp); 
    BOOST_TEST(tmp == 1);
    q.pop(tmp);
    BOOST_TEST(tmp == 2);
    q.pop(tmp);
    BOOST_TEST(tmp == 3);
    q.pop(tmp);
    BOOST_TEST(q.empty());
}

BOOST_AUTO_TEST_CASE(spsc_integration_test) {
    chroniqueue::spsc_queue<int> q(5);
    // Testing is_empty and popping
    int tmp = 0;
    BOOST_TEST(q.empty());
    BOOST_TEST(q.size() == 0);
    q.push(100);
    BOOST_TEST(q.size() == 1);
    BOOST_TEST(!q.empty());
    q.pop(tmp);
    BOOST_TEST(q.empty());
    BOOST_TEST(q.size() == 0);

    // Checking full and if front works properly
    BOOST_TEST(!q.full());
    q.push(25);
    BOOST_TEST(q.front() == 25);
    BOOST_TEST(!q.full());
    q.push(28);
    BOOST_TEST(q.front() == 25);
    BOOST_TEST(!q.full());
    q.push(30);
    BOOST_TEST(!q.full());
    q.push(32);
    BOOST_TEST(!q.full());
    q.push(35);
    BOOST_TEST(q.full());
    int p1 = q.front();
    q.pop(tmp);
    BOOST_TEST(p1 == 25);
    int p2 = q.front();
    q.pop(tmp);
    BOOST_TEST(p2 == 28);
    int p3 = q.front();
    q.pop(tmp);
    BOOST_TEST(p3 == 30);
    BOOST_TEST(q.size() == 2);
    BOOST_TEST(!q.empty());
    
    // Checking the wrap-around write
    q.push(70);
    BOOST_TEST(q.size() == 3);
    q.push(71);
    BOOST_TEST(q.size() == 4);
    q.push(72);
    BOOST_TEST(q.size() == 5);
    int p4 = q.front();
    q.pop(tmp);
    int p5 = q.front();
    q.pop(tmp);
    int p6 = q.front();
    q.pop(tmp);
    BOOST_TEST(p4 == 32);
    BOOST_TEST(p5 == 35);
    BOOST_TEST(p6 == 70);

    // Checking pushing a LOT of times.
    q.push(1);
    BOOST_TEST(q.size() == 3);
    q.push(2);
    BOOST_TEST(q.size() == 4);
    q.push(3);
    BOOST_TEST(q.size() == 5);
    q.push(4); // Shouldn't run
    q.push(5); // Shouldn't run
    q.push(6); // Shouldn't run
    q.pop(tmp);
    BOOST_TEST(tmp == 71);
    q.pop(tmp);
    BOOST_TEST(tmp == 72);
    q.pop(tmp); 
    BOOST_TEST(tmp == 1);
    q.pop(tmp);
    BOOST_TEST(tmp == 2);
    q.pop(tmp);
    BOOST_TEST(tmp == 3);
    q.pop(tmp);
    BOOST_TEST(q.empty());
}

void Thread2Push(chroniqueue::spsc_queue<int> &q, int reps) {
    for (int i = 0; i < reps; i++) {
        while (!q.push(i));
    }
}

BOOST_AUTO_TEST_CASE(spsc_lo_hi) {
    int LO = (int)1e5;
    int HI = (int)1e6;
    for (int nt = 0; nt < 20; nt++) {
        chroniqueue::spsc_queue<int> q(HI);
        std::thread thread2(Thread2Push, std::ref(q), LO);
        int tmp = 0;
        long long tot = 0;
        for (int i = 0; i < LO; i++) {
            while (!q.pop(tmp));
            tot += tmp;
        }
        thread2.join();
        BOOST_TEST(tot == 4999950000);
        BOOST_TEST(q.size() == 0);
    }
}

BOOST_AUTO_TEST_CASE(spsc_hi_lo) {
    int LO = (int)1e5;
    int HI = (int)1e6;
    for (int nt = 0; nt < 20; nt++) {
        chroniqueue::spsc_queue<int> q(LO);
        std::thread thread2(Thread2Push, std::ref(q), HI);
        int tmp = 0;
        long long tot = 0;
        for (int i = 0; i < HI; i++) {
            while (!q.pop(tmp));
            tot += tmp;
        }
        thread2.join();
        BOOST_TEST(tot == 499999500000);
        BOOST_TEST(q.size() == 0);
    }
}

int main(int argc, char **argv) {
    int error_code = boost::unit_test::unit_test_main(init_unit_test, argc, argv);
    if (error_code != 0) return error_code;
    if (STATS_ON && MULTITHREAD_TEST) {

    }
    if (STATS_ON && !MULTITHREAD_TEST) {
        const int NUMCASES = (int)100;
        const int CASELIM = (int)1e7;
        Timer t("../stats/SPSC-PushPop-Singlethread");
        int tmp = 0;
        long long sum;

        std::random_device randev;
        std::mt19937 gen(randev());
        std::uniform_int_distribution<int>  distr(1, 1000);

        std::vector<int> rands(CASELIM);
        sum = 0;
        for (int i = 0; i < NUMCASES; i++) {
            chroniqueue::spsc_queue<int> q(CASELIM);
            rands.clear();
            // Generate random numbers
            for (int j = 0; j < CASELIM; j++) {
                rands.push_back(distr(gen));
            }
            int ct = 0;
            t.start();
            // Push numbers, then pop some of them, then push more, then pop
            // till empty. While doing this, calculate the sum.
            for (int j = 0; j < CASELIM/2; j++) {
                q.push(rands[ct]);
                ct++;
            }
            for (int j = 0; j < CASELIM/4; j++) {
                q.pop(tmp);
                sum += tmp;
            }
            for (int j = 0; j < CASELIM/2; j++) {
                q.push(rands[ct]);
                ct++;
            }
            while (!q.empty()) {
                q.pop(tmp);
                sum += tmp;
            }
            t.stop();
        }
        t.printStats();
        std::cout << "DISREGARD: " << sum << '\n';

        sum = 0;
        t.reset("../stats/Mutex-PushPop-Singlethread");
        for (int i = 0; i < NUMCASES; i++) {
            chroniqueue::mutex_queue<int> q(CASELIM);
            rands.clear();
            // Generate random numbers
            for (int j = 0; j < CASELIM; j++) {
                rands.push_back(distr(gen));
            }
            int ct = 0;
            t.start();
            // Push numbers, then pop some of them, then push more, then pop
            // till empty. While doing this, calculate the sum.
            for (int j = 0; j < CASELIM/2; j++) {
                q.push(rands[ct]);
                ct++;
            }
            for (int j = 0; j < CASELIM/4; j++) {
                q.pop(tmp);
                sum += tmp;
            }
            for (int j = 0; j < CASELIM/2; j++) {
                q.push(rands[ct]);
                ct++;
            }
            while (!q.empty()) {
                q.pop(tmp);
                sum += tmp;
            }
            t.stop();
        }
        t.printStats();
        std::cout << "DISREGARD: " << sum << '\n';
        sum = 0;
        t.reset("../stats/Boost-PushPop-Singlethread");
        for (int i = 0; i < NUMCASES; i++) {
            boost::lockfree::spsc_queue<int> q(CASELIM);
            rands.clear();
            // Generate random numbers
            for (int j = 0; j < CASELIM; j++) {
                rands.push_back(distr(gen));
            }
            int ct = 0;
            t.start();
            // Push numbers, then pop some of them, then push more, then pop
            // till empty. While doing this, calculate the sum.
            for (int j = 0; j < CASELIM/2; j++) {
                q.push(rands[ct]);
                ct++;
            }
            for (int j = 0; j < CASELIM/4; j++) {
                q.pop(tmp);
                sum += tmp;
            }
            for (int j = 0; j < CASELIM/2; j++) {
                q.push(rands[ct]);
                ct++;
            }
            while (!q.empty()) {
                q.pop(tmp);
                sum += tmp;
            }
            t.stop();
        }
        t.printStats();
        std::cout << "DISREGARD SUM: " << sum << '\n';
    }
    return 0;
}