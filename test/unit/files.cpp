//
// Created by gaeqs on 08/05/2025.
//

#include <catch2/catch_all.hpp>
#include <neon/Neon.h>
#include <neon/util/FileUtils.h>

TEST_CASE("Config folder")
{
    auto configFolder = neon::getConfigPath();
    REQUIRE(configFolder.has_value());
    neon::debug() << configFolder.value();

    auto result = neon::createConfigDirectory("neon");
    REQUIRE(result.isOk());
    neon::debug() << result.getResult();
    std::filesystem::remove_all(result.getResult());
}