//
// Created by gaeqs on 23/03/2025.
//

#include "PluginLoader.h"

#include <neon/logging/Logger.h>

#define PLUGINLOADER_H

namespace neon {
    PluginLoader::PluginLoader() {}

    void PluginLoader::loadPlugin(const File& file) {
        const uint8_t* data = reinterpret_cast<const uint8_t*>(file.getData());
        auto manifest = extism::Manifest::wasmBytes(data, file.getSize());
        extism::Plugin plugin(manifest, true);

        auto out = plugin.call("count_vowels", "Hello world!");
        neon::debug() << out.string();
    }

    void PluginLoader::loadPluginURL(const std::string& url) {
        auto manifest = extism::Manifest::wasmURL(url);
        extism::Plugin plugin(manifest, true);

        auto out = plugin.call("count_vowels", "Hello world!");
        neon::debug() << out.string();
    }
}
