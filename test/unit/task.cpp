//
// Created by gaeqs on 24/08/24.
//

#include <iostream>
#include <catch2/catch_all.hpp>

#include <neon/util/task/TaskRunner.h>

TEST_CASE("Task wait", "[task]")
{
    neon::TaskRunner runner;

    std::atomic_bool finished = false;

    std::function func = [&finished] {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        finished = true;
    };

    auto task = runner.executeAsync(func);
    task->wait();

    REQUIRE(finished);
    REQUIRE(task->hasFinished());
}

TEST_CASE("Task parameters", "[task]")
{
    neon::TaskRunner runner;

    std::atomic_bool finished = false;

    std::function func = [&finished](std::string a) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        finished = true;
        return a;
    };

    auto task1 = runner.executeAsync(func, "1");
    auto task2 = runner.executeAsync(func, "2");

    std::string test = "3";
    auto task3 = runner.executeAsync(func, test);
    REQUIRE(test == "3");
    auto task4 = runner.executeAsync(func, std::move(test));
    REQUIRE(test.empty());

    task1->wait();
    task2->wait();
    task3->wait();
    task4->wait();

    REQUIRE(finished);
}

TEST_CASE("Task parameters unique_ptr", "[task]")
{
    neon::TaskRunner runner;

    std::atomic_bool finished1 = false;
    std::atomic_bool finished2 = false;

    std::function func = [&finished1](std::unique_ptr<int> a) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        finished1 = true;
        return std::move(a);
    };

    std::function func2 = [&finished2](std::unique_ptr<int> a) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        finished2 = true;
        return std::move(a);
    };

    std::unique_ptr<int> ptr = std::make_unique<int>(42);
    auto task = runner.executeAsync(func, std::move(ptr));
    task->wait();
    std::unique_ptr<int> result = task->moveResult().value();
    REQUIRE(*result == 42);

    std::unique_ptr<int> ptr2 = std::make_unique<int>(42);
    auto task2 = runner.executeAsync(func2, std::move(ptr2));
    task2->wait();

    REQUIRE(finished1);
    REQUIRE(finished2);
}

TEST_CASE("TaskRunner shutdown", "[task]")
{
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

TEST_CASE("Task return", "[task]")
{
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

TEST_CASE("Task return move", "[task]")
{
    neon::TaskRunner runner;

    std::function func = [] {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return std::make_unique<int>(42);
    };

    auto task = runner.executeAsync(func);

    task->wait();

    auto&& result = task->getResult();
    REQUIRE(result.has_value());

    std::unique_ptr<int> ptr = std::move(task->getResult().value());
    REQUIRE(*ptr == 42);
}

TEST_CASE("Task on main thread", "[task]")
{
    neon::TaskRunner runner;

    std::atomic_bool finished = false;

    std::function func = [&finished] { finished = true; };

    auto task = runner.executeOnMainThread(func);

    REQUIRE(!finished);
    REQUIRE(!task->hasFinished());

    runner.flushMainThreadTasks();

    REQUIRE(finished);
    REQUIRE(task->hasFinished());
}

TEST_CASE("Consecutive tasks", "[task]")
{
    neon::TaskRunner runner;

    std::atomic_bool finished = false;
    std::atomic_bool finished2 = false;

    std::function func = [&finished] {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        finished = true;
    };

    std::function func2 = [&finished, &finished2] {
        REQUIRE(finished);
        finished2 = true;
    };

    std::function func3 = [&finished, &finished2] {
        REQUIRE(finished);
        REQUIRE(finished2);
    };

    auto task = runner.executeAsync(func);
    auto task3 = task->then(func2)->then(func3);

    task->wait();
    task3->wait();

    runner.shutdown();
}
