//
// Created by gaeqs on 24/08/24.
//

#include <iostream>
#include <catch2/catch_all.hpp>

#include <neon/util/task/Coroutine.h>
#include <neon/util/task/TaskRunner.h>


neon::Coroutine<int> coroutine1() {
    co_yield neon::WaitForNextFrame();
    co_return 42;
}

neon::Coroutine<int> coroutineCancel() {
    co_yield neon::WaitForNextFrame();

    FAIL("Coroutine has been resumed");
    co_return 42;
}


struct CoroutineHolder {
    int value;

    explicit CoroutineHolder(int value) : value(value) {}

    neon::Coroutine<int> coroutine() {
        co_yield neon::WaitForNextFrame();
        co_return value;
    }
};

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

TEST_CASE("Coroutine struct", "[coroutine]") {
    neon::TaskRunner runner;

    // Beware! The coroutine will continue its
    // execution even if the holder is destroyed.
    CoroutineHolder holder(20);

    neon::Coroutine<int> coroutine = holder.coroutine();
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
    REQUIRE(task->getResult() == 20);
}

TEST_CASE("Coroutine cancel", "[coroutine]") {
    neon::TaskRunner runner;

    neon::Coroutine<int> coroutine = coroutineCancel();
    auto task = coroutine.asTask();

    REQUIRE(coroutine.isValid());
    runner.launchCoroutine(std::move(coroutine));
    REQUIRE(!coroutine.isValid());

    REQUIRE(!task->hasFinished());
    REQUIRE(!task->isCancelled());
    REQUIRE(!task->getResult().has_value());

    runner.flushMainThreadTasks(); // co_yield
    REQUIRE(!task->hasFinished());
    REQUIRE(!task->isCancelled());
    REQUIRE(!task->getResult().has_value());

    task->cancel();

    runner.flushMainThreadTasks(); // co_return
    REQUIRE(!task->hasFinished());
    REQUIRE(task->isCancelled());
    REQUIRE(!task->getResult().has_value());
}
