//
// Created by gaeqs on 25/10/2024.
//

#include <iostream>
#include <catch2/catch_all.hpp>
#include <neon/filesystem/CMRCFileSystem.h>
#include <neon/loader/AssetLoader.h>
#include <neon/loader/AssetLoaderCollection.h>
#include <neon/render/model/Model.h>
#include <nlohmann/json.hpp>

CMRC_DECLARE(resources);

TEST_CASE("Load test") {
    neon::CMRCFileSystem fileSystem(cmrc::resources::get_filesystem());

    REQUIRE(fileSystem.exists("test.json"));

    auto file = fileSystem.readFile("test.json");

    REQUIRE(file.has_value());

    auto optional = file->toJson();
    REQUIRE(optional.has_value());

    auto json = optional.value();

    REQUIRE(json["foo"].is_object());
    REQUIRE(json["foo"]["bar"].is_number());
    REQUIRE(json["foo"]["bar"].is_number_integer());
    REQUIRE(json["/foo/bar"_json_pointer].is_number_integer());
}

TEST_CASE("Load incorrect") {
    neon::CMRCFileSystem fileSystem(cmrc::resources::get_filesystem());

    REQUIRE(fileSystem.exists("test.txt"));

    auto file = fileSystem.readFile("test.txt");

    REQUIRE(file.has_value());
    auto optional = file->toJson();
    REQUIRE(!optional.has_value());
}

class ModelLoader : public neon::AssetLoader<neon::Model> {
public:
    ModelLoader() = default;

    ModelLoader(const ModelLoader& other) = delete;

    ModelLoader(ModelLoader&& other) noexcept {}

    ModelLoader& operator=(ModelLoader&& other) {
        return *this;
    }

    std::shared_ptr<neon::Model> loadAsset(nlohmann::json json, neon::AssetLoaderContext context) override {
        return nullptr;
    }
};

TEST_CASE("Loader collection") {
    neon::AssetLoaderCollection collection(false);
    std::unique_ptr<neon::AssetLoader<neon::Model>> loader = std::make_unique<ModelLoader>();
    collection.registerLoader<neon::Model>(std::move(loader));

    auto modelLoader = collection.getLoaderFor<neon::Model>();
    REQUIRE(modelLoader.has_value());
}
