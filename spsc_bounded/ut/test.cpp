//
// Created by General Suslik on 11.11.2025.
//

#include <catch2/catch_test_macros.hpp>

#include "spsc_bounded/spsc_bounded.h"

#include <algorithm>
#include <iostream>
#include <thread>
#include <ranges>

TEST_CASE("Enqueue") {
    SPSCBoundedQueue<int> queue{2};
    REQUIRE(queue.Enqueue(2));
    REQUIRE(queue.Enqueue(2));
    REQUIRE_FALSE(queue.Enqueue(2));
}

TEST_CASE("Dequeue") {
    int x;
    SPSCBoundedQueue<int> queue{2};
    REQUIRE_FALSE(queue.Dequeue(x));
    REQUIRE_FALSE(queue.Dequeue(x));
}

TEST_CASE("EnqueueDequeue") {
    SPSCBoundedQueue<int> queue{2};
    REQUIRE(queue.Enqueue(1));
    int val;
    REQUIRE(queue.Dequeue(val));
    REQUIRE(val == 1);
    REQUIRE_FALSE(queue.Dequeue(val));

    REQUIRE(queue.Enqueue(2));
    REQUIRE(queue.Enqueue(3));
    REQUIRE_FALSE(queue.Enqueue(4));

    REQUIRE(queue.Dequeue(val));
    REQUIRE(val == 2);
    REQUIRE(queue.Dequeue(val));
    REQUIRE(val == 3);

    REQUIRE_FALSE(queue.Dequeue(val));
}

TEST_CASE("Threads") {
    static constexpr size_t kN = 1000000;
    SPSCBoundedQueue<int> queue{1024};
    std::vector<int> arr(kN);

    std::thread producer([&]{
        for (int i = 0; i < kN; ++i) {
            while (!queue.Enqueue(i)) {
                std::this_thread::yield();
            }
            ++arr[i];
        }
    });

    std::thread consumer([&]{
        int val;
        for (int i = 0; i < kN; ++i) {
            while (!queue.Dequeue(val)) {
                std::this_thread::yield();
            }
            --arr[val];
            REQUIRE(val == i);
        }
    });

    producer.join();
    consumer.join();

    REQUIRE(std::ranges::all_of(arr, [](const auto& a) { return a == 0; }));
}