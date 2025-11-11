//
// Created by General Suslik on 03.11.2025.
//

#pragma once

#include <atomic>
#include <cassert>
#include <thread>
#include <vector>

template <typename T>
class MPMCBoundedQueue {
public:
    explicit MPMCBoundedQueue(size_t capacity)
        : buffer_(capacity)
        , buffer_mask_(capacity - 1)
        , capacity_(capacity)
    {
        assert(buffer_.size() >= 2 && (buffer_.size() & buffer_mask_) == 0);
        for (size_t i = 0; i < buffer_.size(); ++i) {
            buffer_[i].generation.store(i, std::memory_order_relaxed);
        }
        head_pos_.store(0, std::memory_order_relaxed);
        head_pos_.store(0, std::memory_order_relaxed);
    }

    MPMCBoundedQueue(const MPMCBoundedQueue&) = delete;
    MPMCBoundedQueue& operator=(const MPMCBoundedQueue&) = delete;

    MPMCBoundedQueue(MPMCBoundedQueue&&) = default;
    MPMCBoundedQueue& operator=(MPMCBoundedQueue&&) = default;

    ~MPMCBoundedQueue() = default;

    bool Enqueue(const T& value) {
        for (;;) {
            size_t tail = tail_pos_.load(std::memory_order_acquire);
            auto& node = buffer_[tail & buffer_mask_];
            const int64_t diff = node.generation.load(std::memory_order_relaxed) - tail;
            if (diff < 0) {
                return false;
            }

            if (diff == 0 && tail_pos_.compare_exchange_weak(tail, tail + 1)) {
                node.value = value;
                node.generation.store(tail + 1, std::memory_order_release);
                return true;
            }
        }
    }

    bool Dequeue(T& val) {
        for (;;) {
            size_t head = head_pos_.load(std::memory_order_relaxed);
            auto& node = buffer_[head & buffer_mask_];
            const int64_t diff = node.generation.load(std::memory_order_acquire) - (head + 1);
            if (diff < 0) {
                return false;
            }

            if (diff == 0 && head_pos_.compare_exchange_weak(head, head + 1)) {
                val = std::move(node.value);
                node.generation.store(head + capacity_, std::memory_order_release);
                return true;
            }
        }
    }

private:
    static constexpr size_t cache_line_size = std::hardware_destructive_interference_size;

    struct alignas(cache_line_size) Node {
        std::atomic<size_t> generation;
        T value;
    };

    std::vector<Node> buffer_;
    const size_t buffer_mask_;
    const size_t capacity_;

    alignas(cache_line_size) std::atomic<size_t> tail_pos_;
    alignas(cache_line_size) std::atomic<size_t> head_pos_;
};
