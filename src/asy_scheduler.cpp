#include "asy_scheduler.h"
#include <signal.h>

using namespace asy;

static void on_quit(int sig) {
    scheduler::inst()->quit(1);
}

scheduler* scheduler::inst() {
    static scheduler s_inst;
    return &s_inst;
}

int scheduler::run(int (*main)(int, char*[]), int argc, char* argv[]) {
    signal(SIGINT, on_quit); // ctrl + c
    signal(SIGTERM, on_quit); // kill
    signal(SIGQUIT, on_quit); // ctrl + '\'
    signal(SIGCHLD, SIG_IGN);

    start_coroutine([this, main, argc, argv](){
        int ret = main(argc, argv);
        quit(ret);
    });

    _run_flag = true;
    for (auto& obj : _executers) {
        obj.run(this);
    }
    for (auto& obj : _executers) {
        obj.join();
    }
    return _code;
}

void scheduler::activate() {
    while (_run_flag) {
        bool idle = true;
        while (true) {
            box::object obj;
            if (!_requests.pop(obj)) {
                break;
            }

            idle = false;
            auto type = obj.load<int>();
            on_request(type, obj);
        }

        if (idle) {
            struct timespec req;
            req.tv_sec = 0;
            req.tv_nsec = 10'000'000;
            nanosleep(&req, nullptr);
        }
    }
}

std::shared_ptr<coroutine> scheduler::start_coroutine(const coroutine::func_t& func) {
    auto co = std::make_shared<coroutine>(-1, func);
    _coroutines.push(co);
    return co;
}

std::shared_ptr<coroutine> scheduler::pop_coroutine() {
    std::shared_ptr<coroutine> co;
    _coroutines.pop(co);
    return co;
}

void scheduler::on_request(int type, const box::object& obj) {
    switch (type) {
        case sch_test:
            obj.invoke(std::bind(&scheduler::on_test, this));
            break;
    }
}

void scheduler::on_test(int a, int b) {

}
