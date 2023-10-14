#define BOOST_TEST_MODULE ALL TESTS
#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_ALTERNATIVE_INIT_API
#include <boost/test/included/unit_test.hpp>

#include <iostream>
#include <iterator>
#include <algorithm>

#include "chroniqueue/mutex_queue.hpp"
#include "chroniqueue/spsc_queue.hpp"
#include "chroniqueue/Timer.h"

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

int main(int argc, char **argv) {
    int error_code = boost::unit_test::unit_test_main(init_unit_test, argc, argv);
    if (error_code != 0) return error_code;

    const int NUMCASES = (int)100;
    int CASELIM = (int)1e6;
    Timer t("../stats/SPSC-PushPop");
    int tmp = 0;
    long long sum = 0;
    for (int i = 0; i < NUMCASES; i++) {
        chroniqueue::spsc_queue<int> q(CASELIM);
        t.start();
        for (int j = 0; j < CASELIM/2; j++) {
            q.push(j);
        }
        for (int j = 0; j < CASELIM/4; j++) {
            q.pop(tmp);
        }
        for (int j = 0; j < CASELIM/2; j++) {
            q.push(j);
        }
        while (!q.empty()) q.pop(tmp);
        t.stop();
        sum += tmp;
        CASELIM--;
    }
    t.printStats();
    std::cout << "DISREGARD: " << tmp << '\n';

    CASELIM = (int)1e6;
    t.reset("../stats/Mutex-PushPop");
    for (int i = 0; i < NUMCASES; i++) {
        chroniqueue::mutex_queue<int> q(CASELIM);
        t.start();
        for (int j = 0; j < CASELIM/2; j++) {
            q.push(j);
        }
        for (int j = 0; j < CASELIM/4; j++) {
            q.pop(tmp);
        }
        for (int j = 0; j < CASELIM/2; j++) {
            q.push(j);
        }
        while (!q.empty()) q.pop(tmp);
        t.stop();
        sum += tmp;
        CASELIM--;
    }
    t.printStats();
    std::cout << "DISREGARD: " << tmp << '\n';
    std::cout << "DISREGARD SUM: " << sum << '\n';
    return 0;
}