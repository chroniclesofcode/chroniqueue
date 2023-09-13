#include <boost/lambda/lambda.hpp>
#include <iostream>
#include <iterator>
#include <algorithm>

#include "chroniqueue/threadsafe_queue.hpp"
#include "chroniqueue/lockfree_queue.hpp"
#include "chroniqueue/Timer.h"

int main()
{
    using namespace boost::lambda;
    typedef std::istream_iterator<int> in;

    std::for_each(
        in(std::cin), in(), std::cout << (_1 * 3) << " " );
}