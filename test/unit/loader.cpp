//
// Created by gaeqs on 25/10/2024.
//

#include <iostream>
#include <catch2/catch_all.hpp>
#include <neon/assimp/AssimpScene.h>
#include <neon/filesystem/CMRCFileSystem.h>
#include <neon/filesystem/DirectoryFileSystem.h>
#include <neon/loader/AssetLoader.h>
#include <neon/loader/AssetLoaderCollection.h>
#include <neon/loader/AssetLoaderHelpers.h>
#include <neon/render/buffer/SimpleFrameBuffer.h>
#include <neon/render/model/Model.h>
#include <nlohmann/json.hpp>

CMRC_DECLARE(resources);

TEST_CASE("Load test")
{
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

TEST_CASE("Load incorrect")
{
    neon::CMRCFileSystem fileSystem(cmrc::resources::get_filesystem());

    REQUIRE(fileSystem.exists("/a/../test.txt"));
    auto file = fileSystem.readFile("test.txt");

    REQUIRE(file.has_value());
    auto optional = file->toJson();
    REQUIRE(!optional.has_value());
}

class ModelLoader : public neon::AssetLoader<neon::Model>
{
  public:
    ModelLoader() = default;

    ModelLoader(const ModelLoader& other) = delete;

    ModelLoader(ModelLoader&& other) noexcept
    {
    }

    ModelLoader& operator=(ModelLoader&& other)
    {
        return *this;
    }

    std::shared_ptr<neon::Model> loadAsset(std::string name, nlohmann::json json,
                                           neon::AssetLoaderContext context) override
    {
        return nullptr;
    }
};

TEST_CASE("Loader collection")
{
    neon::AssetLoaderCollection collection(false);
    std::unique_ptr<neon::AssetLoader<neon::Model>> loader = std::make_unique<ModelLoader>();
    collection.registerLoader<neon::Model>(std::move(loader));

    auto modelLoader = collection.getLoaderFor<neon::Model>();
    REQUIRE(modelLoader.has_value());
}

TEST_CASE("Load shader")
{
    neon::vulkan::VKApplicationCreateInfo info;
    info.name = "Neon";
    info.windowSize = {800, 600};

    neon::Application application(std::make_unique<neon::vulkan::VKApplication>(info));
    application.init();

    neon::CMRCFileSystem fileSystem(cmrc::resources::get_filesystem());
    neon::AssetLoaderContext context(&application, nullptr, &fileSystem);

    auto shader = neon::loadAssetFromFile<neon::ShaderProgram>("shader.json", context);
    REQUIRE(shader != nullptr);

    REQUIRE(context.collection->get(shader->getIdentifier()).has_value());

    neon::logger.debug(neon::MessageBuilder().print("Shader loaded?: ").print(shader != nullptr));
    neon::logger.debug(neon::MessageBuilder().print("Shader identifier: ").print(shader->getIdentifier().name));

    auto* impl = dynamic_cast<neon::vulkan::VKApplication*>(application.getImplementation());
    impl->finishLoop();
}

TEST_CASE("Load material")
{
    neon::vulkan::VKApplicationCreateInfo info;
    info.name = "Neon";
    info.windowSize = {800, 600};

    neon::Application application(std::make_unique<neon::vulkan::VKApplication>(info));
    application.init();

    neon::CMRCFileSystem fileSystem(cmrc::resources::get_filesystem());
    neon::AssetLoaderContext context(&application, nullptr, &fileSystem);

    std::shared_ptr<neon::SimpleFrameBuffer> fb = std::make_shared<neon::SimpleFrameBuffer>(
        &application, "frame_buffer", neon::SamplesPerTexel::COUNT_1, std::vector<neon::FrameBufferTextureCreateInfo>(),
        neon::FrameBufferDepthCreateInfo());

    application.getAssets().store(fb, neon::AssetStorageMode::PERMANENT);

    auto material = neon::loadAssetFromFile<neon::Material>("material.json", context);
    REQUIRE(material != nullptr);

    REQUIRE(context.collection->get(material->getIdentifier()).has_value());
    REQUIRE(material->getTarget() == fb);

    neon::logger.debug(neon::MessageBuilder().print("Material loaded?: ").print(material != nullptr));
    neon::logger.debug(neon::MessageBuilder().print("Material identifier: ").print(material->getIdentifier().name));

    auto* impl = dynamic_cast<neon::vulkan::VKApplication*>(application.getImplementation());
    impl->finishLoop();
}

TEST_CASE("Load assimp")
{
    neon::vulkan::VKApplicationCreateInfo info;
    info.name = "Neon";
    info.windowSize = {800, 600};

    neon::Application application(std::make_unique<neon::vulkan::VKApplication>(info));
    application.init();

    neon::DirectoryFileSystem fileSystem("resource");
    neon::AssetLoaderContext context(&application, nullptr, &fileSystem);

    auto scene = neon::loadAssetFromFile<neon::AssimpScene>("sans.json", context);

    auto* impl = dynamic_cast<neon::vulkan::VKApplication*>(application.getImplementation());
    impl->finishLoop();
}
