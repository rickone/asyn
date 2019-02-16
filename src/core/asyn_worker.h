#pragma once

#include <pthread.h>
#include <list>
#include <memory>
#include <unordered_map>
#include "asyn_coroutine.h"
#include "asyn_timer.h"
#include "asyn_poller.h"
#include "asyn_lockfree_queue.h"

namespace asyn {

#define MIN_CO_COUNT 16
#define TIMESLICE_NANOSEC 10'000'000ll

class worker {
public:
    worker() = default;
    ~worker() = default;

    static worker* current();

    void run();
    void join();
    void bind_cpu_core(int cpu_core);
    void pause();
    void on_thread(coroutine* self);
    void on_step();
    void process_paused_coroutines();

    coroutine* co_self() { return _self; }
    void set_co_self(coroutine* self) { _self = self; }
    timer* timer_inst() { return &_timer; }
    poller* poller_inst() { return &_poller; }

private:
    pthread_t _thread = (pthread_t)0;
    coroutine* _self = nullptr;
    timer _timer;
    poller _poller;
    std::list<std::shared_ptr<coroutine>> _coroutines;
    std::unordered_map<int, std::shared_ptr<coroutine>> _paused_coroutines;
    int _max_co_count = 0;
    int64_t _timeslice_ns = 0;
};

} // asyn