#pragma once

#include <atomic>

namespace sco {

template<typename T>
class lockfree_queue {
public:
    struct node_t {
        node_t() : refcnt(1) {
        }
        node_t(const T& t) : refcnt(2), value(t) {
        }
        ~node_t() = default;

        void release() {
            int cnt = --refcnt;
            if (cnt == 0) {
                delete this;
            }
        }

        node_t* next = nullptr;
        std::atomic<int> refcnt;
        T value;
    };

    lockfree_queue() {
        auto dummy = new node_t();
        _head = dummy;
        _tail = dummy;
    }

    ~lockfree_queue() {
        for (auto node = _head.load(); node != nullptr; ) {
            auto next = node->next;
            delete node;
            node = next;
        }
        _head = nullptr;
        _tail = nullptr;
    }

    lockfree_queue(const lockfree_queue&) = delete;
    lockfree_queue(lockfree_queue&&) = delete;
    lockfree_queue& operator=(const lockfree_queue&) = delete;
    lockfree_queue& operator=(lockfree_queue&&) = delete;

    void push(const T& t) {
        node_t* new_node = new node_t(t);
        node_t* node = _tail.load(std::memory_order_consume);

        while (!_tail.compare_exchange_weak(node, new_node, std::memory_order_release, std::memory_order_consume));

        node->next = new_node;
    }

    bool pop(T& value) {
        node_t* node = _head.load(std::memory_order_consume);
        node_t* next = nullptr;
        do {
            next = node->next;
            if (!next)
                return false;

        } while (!_head.compare_exchange_weak(node, next, std::memory_order_release, std::memory_order_consume));

        value = next->value;
        next->release();

        node->release();
        return true;
    }

private:
    std::atomic<node_t*> _head;
    std::atomic<node_t*> _tail;
};

} // sco
