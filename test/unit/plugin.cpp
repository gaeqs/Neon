//
// Created by gaeqs on 23/03/2025.
//

#include <catch2/catch_all.hpp>
#include <neon/plugin/PluginLoader.h>

TEST_CASE("Plugin", "[plugin]") {
    neon::PluginLoader loader;

    loader.loadPluginURL("https://github.com/extism/plugins/releases/"
        "latest/download/count_vowels.wasm");
}
