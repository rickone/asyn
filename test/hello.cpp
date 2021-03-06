#include "sco.h"
#include <cstdio>

using namespace std::chrono_literals;

void foo(int i) {
    printf("[%d] Hello World!\n", i);
}

int main() {
    sco::scheduler::attach();

    for (int i = 0; i < 20; i++) {
        sco::start(std::bind(foo, i));
        sco::sleep_for(10ms);
    }
    return 0;
}
