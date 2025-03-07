//
// Created by gaeqs on 24/08/24.
//

#include <iostream>
#include <catch2/catch_all.hpp>
#include <neon/logging/Logger.h>
#include <neon/logging/Message.h>
#include <neon/logging/STDLogOutput.h>

#include <neon/util/task/Coroutine.h>
#include <neon/util/task/TaskRunner.h>


TEST_CASE("Logger basic", "[logging]") {
    using Effect = neon::TextEffect;
    neon::Logger logger;

    neon::MessageBuilder builder;
    builder.group("warning");
    builder.effect(Effect::background4bits(10));
    builder.effect(Effect::foregroundRGB(0x55, 0xDD, 0x55));
    builder.print("Hello");
    builder.effect(Effect::defaultBackground());
    builder.print(" ");
    builder.effect(Effect::background4bits(2));
    builder.print("world");
    logger.print(builder.build());
}

TEST_CASE("Logger default groups", "[logging]") {
    neon::Logger logger;

    logger.info("Info");
    logger.done("Done");
    logger.debug("Debug");
    logger.warning("Warning");
    logger.error("Error");
}

TEST_CASE("Logger operators", "[logging]") {
    neon::error() << "Hello" << " " << "world";

    for (size_t i = 0; i < 10; ++i) {
        neon::debug() << i;
    }

    neon::debug() << rush::Vec3f(1.0f, 3.5f, 5.0f);
}
