#include "asyn.h"
#include <cstdio>

using namespace std::chrono_literals;

static asyn::mutex s_mutex;
static std::vector<int> s_result;

bool is_prime(int n) {
    if (n % 2 == 0) {
        return false;
    }

    int m = (int)sqrt(n);
    for (int i = 3; i <= m; i += 2) {
        if (n % i == 0) {
            return false;
        }
    }

    printf("%d is prime\n", n);
    return true;
}

void foo(int n) {
    if (!is_prime(n)) {
        return;
    }

    s_mutex.lock();
    s_result.push_back(n);
    s_mutex.unlock();
}

int main() {
    for (int i = 2; i < 1000; i++) {
        asyn::start(std::bind(foo, i));
    }

    asyn::sleep_for(1s);

    s_mutex.lock();
    puts("prime number:");
    for (int n : s_result) {
        printf("%d\n", n);
    }
    s_mutex.unlock();

    return 0;
}