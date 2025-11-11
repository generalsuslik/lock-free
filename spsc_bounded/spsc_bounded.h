//
// Created by General Suslik on 11.11.2025.
//

#pragma once

#include <atomic>
#include <cassert>
#include <thread>
#include <vector>

template <typename T>
class SPSCBoundedQueue {
public:
    explicit SPSCBoundedQueue(size_t capacity)
        : buffer_(capacity)
        , buffer_mask_(capacity - 1)
        , capacity_(capacity)
    {
        assert(!buffer_.empty() && (buffer_.size() & buffer_mask_) == 0);
        head_pos_.store(0, std::memory_order_relaxed);
        head_pos_.store(0, std::memory_order_relaxed);
    }

    SPSCBoundedQueue(const SPSCBoundedQueue&) = delete;
    SPSCBoundedQueue& operator=(const SPSCBoundedQueue&) = delete;

    SPSCBoundedQueue(SPSCBoundedQueue&&) = default;
    SPSCBoundedQueue& operator=(SPSCBoundedQueue&&) = default;

    ~SPSCBoundedQueue() = default;

    bool Enqueue(const T& value) noexcept {
        const size_t head = head_pos_.load(std::memory_order_relaxed);
        const size_t tail = tail_pos_.load(std::memory_order_acquire);
        if (tail - head >= capacity_) {
            return false;
        }

        buffer_[tail & buffer_mask_] = std::move(value);
        tail_pos_.store(tail + 1, std::memory_order_release);
        return true;
    }

    bool Dequeue(T& val) noexcept {
        const size_t head = head_pos_.load(std::memory_order_relaxed);
        const size_t tail = tail_pos_.load(std::memory_order_acquire);
        if (head == tail) {
            return false;
        }

        val = std::move(buffer_[head & buffer_mask_]);
        head_pos_.store(head + 1, std::memory_order_release);
        return true;
    }

private:
    static constexpr size_t cache_line_size = std::hardware_destructive_interference_size;

    std::vector<T> buffer_;
    const size_t buffer_mask_;
    const size_t capacity_;

    alignas(cache_line_size) std::atomic<size_t> tail_pos_;
    alignas(cache_line_size) std::atomic<size_t> head_pos_;
};
