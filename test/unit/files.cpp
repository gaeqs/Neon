//
// Created by gaeqs on 08/05/2025.
//

#include <catch2/catch_all.hpp>
#include <neon/Neon.h>
#include <neon/util/FileUtils.h>
#include <neon/util/dialog/Dialogs.h>

TEST_CASE("Config folder", "[files]")
{
    auto configFolder = neon::getConfigPath();
    REQUIRE(configFolder.has_value());
    neon::debug() << configFolder.value();

    auto result = neon::createConfigDirectory("neon");
    REQUIRE(result.isOk());
    neon::debug() << result.getResult();
    std::filesystem::remove_all(result.getResult());
}

TEST_CASE("Open dialog", "[.files]")
{
    neon::OpenFileDialogInfo info;
    info.title = "Neon Open Dialog";
    info.filters = {
        neon::FileFilter{"Text file", "*.txt"},
        neon::FileFilter{ "Any file",     "*"}
    };
    info.multiselect = true;
    info.pickFolders = true;
    auto dialog = neon::openFileDialog(info);
    for (auto& paths : dialog) {
        neon::debug() << paths;
    }
}

TEST_CASE("Save dialog", "[.files]")
{
    neon::SaveFileDialogInfo info;
    info.title = "Neon Open Dialog";
    info.filters = {
        neon::FileFilter{"Text file", "*.txt"},
        neon::FileFilter{ "Any file",     "*"}
    };
    info.defaultFileName = "test";
    info.defaultExtension = "txt";
    if (auto dialog = neon::saveFileDialog(info)) {
        neon::debug() << *dialog;
    }
}
