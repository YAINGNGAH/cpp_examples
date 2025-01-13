#pragma once

#include <atomic>
#include <optional>
#include <stdexcept>
#include <utility>

template <class T>
class MPSCStack {
private:
    struct Node {
        T value;
        Node* next;
    };

    std::atomic<Node*> head_;

public:
    // Push adds one element to stack top.
    //
    // Safe to call from multiple threads.
    void Push(const T& value) {
        Node* next = new Node{std::move(value), nullptr};
        Node* head = head_.load();
        do {
            next->next = head;
        } while (!head_.compare_exchange_weak(head, next));
    }

    // Pop removes top element from the stack.
    //
    // Not safe to call concurrently.
    std::optional<T> Pop() {
        Node* head = head_.load();
        do {
            if (head == nullptr) {
                return std::nullopt;
            }
        } while (!head_.compare_exchange_weak(head, head->next));
        T value = head->value;
        delete head;
        return value;
    }

    // DequeuedAll Pop's all elements from the stack and calls cb() for each.
    //
    // Not safe to call concurrently with Pop()
    template <class TFn>
    void DequeueAll(const TFn& cb) {
        std::optional<T> v = Pop();
        while (v != std::nullopt) {
            cb(*v);
            v = Pop();
        }
    }

    ~MPSCStack() {
        Node* head = head_.load();
        while (head != nullptr) {
            Node* next = head->next;
            delete head;
            head = next;
        }
    }
};
