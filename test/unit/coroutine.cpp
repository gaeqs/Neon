//
// Created by gaeqs on 24/08/24.
//

#include <iostream>
#include <catch2/catch_all.hpp>

#include <util/task/Coroutine.h>
#include <util/task/TaskRunner.h>


neon::Coroutine<int> coroutine1() {
    co_yield neon::WaitForNextFrame();
    co_return 42;
}

TEST_CASE("Coroutine", "[coroutine]") {
    neon::TaskRunner runner;

    neon::Coroutine<int> coroutine = coroutine1();
    auto task = coroutine.asTask();

    REQUIRE(coroutine.isValid());
    runner.launchCoroutine(std::move(coroutine));
    REQUIRE(!coroutine.isValid());

    REQUIRE(!task->hasFinished());
    REQUIRE(!task->getResult().has_value());

    runner.flushMainThreadTasks(); // co_yield
    REQUIRE(!task->hasFinished());
    REQUIRE(!task->getResult().has_value());

    runner.flushMainThreadTasks(); // co_return
    REQUIRE(task->hasFinished());
    REQUIRE(task->getResult().has_value());
    REQUIRE(task->getResult() == 42);
}
