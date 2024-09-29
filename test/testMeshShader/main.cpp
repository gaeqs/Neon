#include <iostream>
#include <memory>
#include <vector>
#include <filesystem>
#include <random>

#include <neon/Neon.h>
#include <neon/util/component/CameraMovementComponent.h>
#include <neon/util/component/DebugOverlayComponent.h>
#include <neon/util/component/DockSpaceComponent.h>
#include <neon/util/component/ViewportComponent.h>
#include <neon/util/component/SceneTreeComponent.h>
#include <neon/util/component/GameObjectExplorerComponent.h>
#include <neon/util/DeferredUtils.h>
#include <neon/util/ModelUtils.h>
#include <neon/assimp/AssimpLoader.h>

#include "TestVertex.h"
#include "GlobalParametersUpdaterComponent.h"
#include "LockMouseComponent.h"
#include "ConstantRotationComponent.h"
#include "neon/render/shader/Material.h"
#include "neon/render/shader/MaterialCreateInfo.h"
#include "neon/render/shader/ShaderUniformBinding.h"


#ifdef USE_VULKAN
#include <vulkan/util/component/VulkanInfoCompontent.h>
#endif

constexpr float WIDTH = 800;
constexpr float HEIGHT = 600;

CMRC_DECLARE(shaders);

using namespace neon;


std::vector<rush::Vec3f> randomPoints(size_t amount) {
    std::vector<rush::Vec3f> vec;
    vec.reserve(amount);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distr(-10.0, 10.0);

    for (size_t i = 0; i < amount; ++i) {
        vec.emplace_back(
            distr(gen),
            distr(gen),
            distr(gen)
        );
    }

    return vec;
}

/**
 * Creates a shader program.
 * @param room the application.
 * @param name the name of the shader.
 * @param vert the vertex shader.
 * @param frag the fragment shader.
 * @return the shader program.
 */
std::shared_ptr<ShaderProgram> createShader(Application* application,
                                            const std::string& name,
                                            const std::string& vert,
                                            const std::string& frag) {
    auto shader = std::make_shared<ShaderProgram>(application, std::move(name));

    auto defaultVert = cmrc::shaders::get_filesystem().open(vert);
    auto defaultFrag = cmrc::shaders::get_filesystem().open(frag);

    shader->addShader(
        ShaderType::VERTEX,
        std::string(defaultVert.begin(), defaultVert.end())
    );

    shader->addShader(
        ShaderType::FRAGMENT,
        std::string(defaultFrag.begin(), defaultFrag.end())
    );

    auto result = shader->compile();
    if (result.has_value()) {
        application->getLogger().error(result.value());
        throw std::runtime_error(result.value());
    }

    return shader;
}

std::shared_ptr<ShaderProgram> createMeshShader(Application* application,
                                                const std::string& name,
                                                const std::string& task,
                                                const std::string& mesh,
                                                const std::string& frag) {
    auto shader = std::make_shared<ShaderProgram>(application, std::move(name));

    auto defaultMesh = cmrc::shaders::get_filesystem().open(mesh);
    auto defaultFrag = cmrc::shaders::get_filesystem().open(frag);

    if (!task.empty()) {
        auto defaultTask = cmrc::shaders::get_filesystem().open(task);
        shader->addShader(
            ShaderType::TASK,
            std::string(defaultTask.begin(), defaultTask.end())
        );
    }

    shader->addShader(
        ShaderType::MESH,
        std::string(defaultMesh.begin(), defaultMesh.end())
    );

    shader->addShader(
        ShaderType::FRAGMENT,
        std::string(defaultFrag.begin(), defaultFrag.end())
    );

    auto result = shader->compile();
    if (result.has_value()) {
        application->getLogger().error(result.value());
        throw std::runtime_error(result.value());
    }

    auto* l = Logger::defaultLogger();
    l->debug(MessageBuilder().print("Shader: ").print(name));
    l->debug(MessageBuilder().print("- Uniforms: ").print(shader->getImplementation().getUniforms().size()));
    l->debug(MessageBuilder().print("- Uniform blocks: ").print(shader->getImplementation().getUniformBlocks().size()));

    for (auto& [name, block] : shader->getImplementation().getUniformBlocks()) {
        l->debug(MessageBuilder().print("  - Name: ").print(name));
        l->debug(MessageBuilder().print("    - Set: ").print(block.set.value_or(-1)));
        l->debug(MessageBuilder().print("    - Binding: ").print(block.binding.value_or(-1)));
    }

    return shader;
}


std::shared_ptr<FrameBuffer> initRender(Room* room) {
    auto* app = room->getApplication();

    // Bindings for the global uniforms.
    // In this application, we have a buffer of global parameters
    // and a skybox.
    std::vector<ShaderUniformBinding> globalBindings = {
        {UniformBindingType::BUFFER, sizeof(Matrices)},
        {UniformBindingType::IMAGE, 0}
    };

    // The description of the global uniforms.
    auto globalDescriptor = std::make_shared<ShaderUniformDescriptor>(
        app, "default", globalBindings);

    // The render of the application.
    // We should set the render to the application before
    // we do anything else, as some components depend on
    // the application's render.
    auto render = std::make_shared<Render>(app, "default", globalDescriptor);
    app->setRender(render);

    auto screenShader = createShader(app,
                                     "screen",
                                     "screen.vert",
                                     "screen.frag");

    std::vector<FrameBufferTextureCreateInfo> frameBufferFormats = {
        TextureFormat::R8G8B8A8
    };

    auto fpFrameBuffer = std::make_shared<SimpleFrameBuffer>(
        app, frameBufferFormats, true);

    render->addRenderPass(std::make_shared<DefaultRenderPassStrategy>(
        fpFrameBuffer));

    std::vector<FrameBufferTextureCreateInfo> screenFormats =
            {TextureFormat::R8G8B8A8};
    auto screenFrameBuffer = std::make_shared<SimpleFrameBuffer>(
        app, screenFormats, false);
    render->addRenderPass(std::make_shared<DefaultRenderPassStrategy>(
        screenFrameBuffer));

    auto textures = fpFrameBuffer->getTextures();

    std::shared_ptr screenMaterial = Material::create(
        room->getApplication(), "Screen Model",
        screenFrameBuffer, screenShader,
        deferred_utils::DeferredVertex::getDescription(),
        InputDescription(0, InputRate::INSTANCE),
        {}, textures);

    auto screenModel = deferred_utils::createScreenModel(
        room->getApplication(),
        ModelCreateInfo(),
        "Screen Model"
    );

    screenModel->addMaterial(screenMaterial);

    auto screenModelGO = room->newGameObject();
    screenModelGO->setName("Screen Model");
    screenModelGO->newComponent<GraphicComponent>(screenModel);

    auto swapFrameBuffer = std::make_shared<SwapChainFrameBuffer>(
        room, false);

    render->addRenderPass(std::make_shared<DefaultRenderPassStrategy>(
        swapFrameBuffer));

    return fpFrameBuffer;
}

void loadModels(Application* application, Room* room,
                const std::shared_ptr<FrameBuffer>& target) {
    std::shared_ptr materialDescriptor =
            ShaderUniformDescriptor::ofImages(application, "default", 2);

    auto shader = createMeshShader(application,
                                   "deferred",
                                   "",
                                   "deferred.mesh",
                                   "deferred.frag");

    std::vector<ShaderUniformBinding> cubeMaterialBindings;

    std::shared_ptr<ShaderUniformDescriptor> cubeMaterialDescriptor;
    materialDescriptor = std::make_shared<ShaderUniformDescriptor>(
        room->getApplication(),
        "pointMaterialDescriptor",
        cubeMaterialBindings
    );

    MaterialCreateInfo cubeMaterialInfo(target, shader);
    cubeMaterialInfo.descriptions.uniform = materialDescriptor;
    cubeMaterialInfo.descriptions.vertex.
            push_back(TestVertex::getDescription());
    cubeMaterialInfo.descriptions.instance.push_back(
        DefaultInstancingData::getInstancingDescription());
    cubeMaterialInfo.topology = PrimitiveTopology::POINT_LIST;
    cubeMaterialInfo.rasterizer.polygonMode = PolygonMode::LINE;
    cubeMaterialInfo.rasterizer.cullMode = CullMode::NONE;

    auto material = std::make_shared<Material>(application, "pointMaterial",
                                               cubeMaterialInfo);

    ModelCreateInfo modelInfo;
    modelInfo.pipeline.type = PipelineType::MESH;

    auto model = model_utils::createModel<rush::Vec3f>(
        room,
        "points",
        material,
        randomPoints(10000),
        modelInfo
    );

    auto object = room->newGameObject();
    object->newComponent<GraphicComponent>(model);
    object->getTransform().setPosition(rush::Vec3f(0.0f, 10.0f, -10.0f));
    object->setName("Object");
}

std::shared_ptr<Texture> loadSkybox(Room* room) {
    static const std::vector<std::string> PATHS = {
        "resource/Skybox/right.jpg",
        "resource/Skybox/left.jpg",
        "resource/Skybox/top.jpg",
        "resource/Skybox/bottom.jpg",
        "resource/Skybox/front.jpg",
        "resource/Skybox/back.jpg",
    };

    TextureCreateInfo info;
    info.imageView.viewType = TextureViewType::CUBE;
    info.image.layers = 6;

    return Texture::createTextureFromFiles(room->getApplication(),
                                           "skybox", PATHS, info);
}

std::shared_ptr<Room> getTestRoom(Application* application) {
    auto room = std::make_shared<Room>(application);

    auto fpFrameBuffer = initRender(room.get());

    auto skybox = loadSkybox(room.get());
    application->getRender()->getGlobalUniformBuffer().setTexture(1, skybox);


    auto cameraController = room->newGameObject();
    auto cameraMovement = cameraController->newComponent<
        CameraMovementComponent>();
    cameraMovement->setSpeed(10.0f);

    auto parameterUpdater = room->newGameObject();
    parameterUpdater->newComponent<GlobalParametersUpdaterComponent>();
    parameterUpdater->newComponent<LockMouseComponent>(cameraMovement);
    parameterUpdater->newComponent<DockSpaceComponent>();
    parameterUpdater->newComponent<ViewportComponent>();
    auto goExplorer = parameterUpdater->newComponent<
        GameObjectExplorerComponent>();
    parameterUpdater->newComponent<SceneTreeComponent>(goExplorer);
    parameterUpdater->newComponent<DebugOverlayComponent>(false, 100);
    parameterUpdater->newComponent<LogComponent>();

#ifdef USE_VULKAN
    parameterUpdater->newComponent<vulkan::VulkanInfoCompontent>();
#endif

    loadModels(application, room.get(), fpFrameBuffer);

    return room;
}

int main() {
    using MeshFeature = VkPhysicalDeviceMeshShaderFeaturesEXT;
    constexpr VkStructureType MESH_FEATURE =
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;

    std::srand(std::time(nullptr));

    vulkan::VKApplicationCreateInfo info;
    info.name = "Neon";
    info.windowSize = {WIDTH, HEIGHT};
    info.vSync = true;
    info.defaultExtensionInclusion = vulkan::InclusionMode::EXCLUDE_ALL;
    info.defaultFeatureInclusion = vulkan::InclusionMode::EXCLUDE_ALL;

    info.extraFeatures.emplace_back(MeshFeature{MESH_FEATURE});

    info.featuresConfigurator = [](const auto& device,
                                   vulkan::VKPhysicalDeviceFeatures& features) {
        vulkan::VKApplicationCreateInfo::
                defaultFeaturesConfigurer(device, features);
        features.basicFeatures.geometryShader = true;
        features.basicFeatures.fillModeNonSolid = true;
        features.basicFeatures.samplerAnisotropy = true;

        auto* mesh = features.findFeature<MeshFeature>(MESH_FEATURE).value();
        mesh->meshShader = true;

        features.extensions.emplace_back(VK_EXT_MESH_SHADER_EXTENSION_NAME);
    };

    info.deviceFilter = [](const vulkan::VKPhysicalDevice& device) {
        if (!vulkan::VKApplicationCreateInfo::defaultDeviceFilter(device))
            return false;
        auto feature = device.getFeatures().findFeature<MeshFeature>(
            MESH_FEATURE);
        if (!feature.has_value()) return false;
        return feature.value()->meshShader > 0;
    };

    info.enableValidationLayers = true;

    Application application(std::make_unique<vulkan::VKApplication>(info));

    application.init();
    application.setRoom(getTestRoom(&application));

    auto loopResult = application.startGameLoop();
    if (loopResult.isOk()) {
        application.getLogger().done(MessageBuilder()
            .print("Application closed. ")
            .print(loopResult.getResult(), TextEffect::foreground4bits(2))
            .print(" frames generated."));
    } else {
        application.getLogger().done(MessageBuilder()
            .print("Unexpected game loop error: ")
            .print(loopResult.getError(), TextEffect::foreground4bits(1)));
    }

    application.setRoom(nullptr);

    return EXIT_SUCCESS;
}
