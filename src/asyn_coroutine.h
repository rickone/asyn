#pragma once

#include <functional>
#include <memory>
#include <ucontext.h>
#include "box.h"

namespace asyn {

const int COROUTINE_DEFAULT_STACK_LEN = 1024 * 256;

enum {
    COROUTINE_UNINIT,
    COROUTINE_RUNNING,
    COROUTINE_SUSPEND,
    COROUTINE_DEAD,
};

class coroutine : public std::enable_shared_from_this<coroutine> {
public:
    typedef std::function<void(void)> func_t;
    
    coroutine() = default;
    coroutine(int id, const func_t& func);
    coroutine(const coroutine& other) = delete;
    coroutine(coroutine&& other) = delete;
    virtual ~coroutine();

    static coroutine* self();

    void init(const func_t& func = nullptr, size_t stack_len = COROUTINE_DEFAULT_STACK_LEN);
    void set_self();
    bool resume();
    void yield();
    void join();
    void detach();

    box::object get_value() { return std::move(_val); }

    template<typename... A>
    void set_value(A... args) {
        _val.store_args(args...);
    }

    void move_value(box::object&& val) {
        _val = std::move(val);
    }

    int id() const { return _id; }
    int status() const { return _status; }

private:
    static void body(coroutine* co);

    int _id = 0;
    func_t _func = nullptr;
    void* _stack = nullptr;
    ucontext_t _ctx;
    int _status = COROUTINE_UNINIT;
    box::object _val;
    int _join_id = 0;
    bool _detach = false;
};

template<typename R, typename... A>
inline std::shared_ptr<coroutine> create_coroutine(const std::function<R (A...)>& func, size_t stack_len = COROUTINE_DEFAULT_STACK_LEN) {
    auto co = std::make_shared<coroutine>();
    std::function<void(void)> real_func = [co, func](){
        auto val = co->get_value();
        auto r = val.call(func);
        co->set_value(r);
    };
    co->init(real_func, stack_len);
    return co;
}

template<typename R, typename... A>
inline std::shared_ptr<coroutine> create_coroutine(R (*func)(A...), size_t stack_len = COROUTINE_DEFAULT_STACK_LEN) {
    auto co = std::make_shared<coroutine>();
    std::function<void(void)> real_func = [co, func](){
        auto val = co->get_value();
        auto r = val.call(func);
        co->set_value(r);
    };
    co->init(real_func, stack_len);
    return co;
}

template<typename... A>
inline std::shared_ptr<coroutine> create_coroutine(const std::function<void (A...)>& func, size_t stack_len = COROUTINE_DEFAULT_STACK_LEN) {
    auto co = std::make_shared<coroutine>();
    std::function<void(void)> real_func = [co, func](){
        auto val = co->get_value();
        val.invoke(func);
    };
    co->init(real_func, stack_len);
    return co;
}

template<typename... A>
inline std::shared_ptr<coroutine> create_coroutine(void (*func)(A...), size_t stack_len = COROUTINE_DEFAULT_STACK_LEN) {
    auto co = std::make_shared<coroutine>();
    std::function<void(void)> real_func = [co, func](){
        auto val = co->get_value();
        val.invoke(func);
    };
    co->init(real_func, stack_len);
    return co;
}

template<typename... A>
inline box::object yield(A... args) {
    auto self = coroutine::self();
    self->set_value(args...);
    self->yield();
    return self->get_value();
}

template<typename... A>
inline box::object resume(const std::shared_ptr<coroutine>& co, A... args) {
    if (co->status() != COROUTINE_SUSPEND)
        return box::object();

    co->set_value(args...);
    co->resume();
    return co->get_value();
}

} // asyn