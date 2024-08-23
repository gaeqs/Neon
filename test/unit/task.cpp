//
// Created by gaeqs on 24/08/24.
//

#include <catch2/catch_all.hpp>

#include "util/task/TaskRunner.h"

TEST_CASE("Task wait", "[task]") {
    neon::TaskRunner runner;

    std::atomic_bool finished = false;

    std::function func = [&finished] {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        finished = true;
    };

    auto task = runner.executeAsync(func);
    task->wait();

    REQUIRE(finished);
}

TEST_CASE("TaskRunner shutdown", "[task]") {
    neon::TaskRunner runner;

    std::atomic_bool finished = false;

    std::function func = [&finished] {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        finished = true;
    };

    auto task = runner.executeAsync(func);

    runner.shutdown();

    REQUIRE(finished);
}

TEST_CASE("Task return", "[task]") {
    neon::TaskRunner runner;

    std::function func = [] {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return 42;
    };

    auto task = runner.executeAsync(func);

    task->wait();

    auto result = task->getResult();

    REQUIRE(result.has_value());
    REQUIRE(result == 42);
}