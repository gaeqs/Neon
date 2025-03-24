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

/**
 * Creates a shader program.
 * @param room the application.
 * @param name the name of the shader.
 * @param vert the vertex shader.
 * @param frag the fragment shader.
 * @return the shader program.
 */
std::shared_ptr<ShaderProgram> createShader(Application* application, const std::string& name, const std::string& vert,
                                            const std::string& frag)
{
    auto shader = std::make_shared<ShaderProgram>(application, std::move(name));

    auto defaultVert = cmrc::shaders::get_filesystem().open(vert);
    auto defaultFrag = cmrc::shaders::get_filesystem().open(frag);

    shader->addShader(ShaderType::VERTEX, std::string(defaultVert.begin(), defaultVert.end()));

    shader->addShader(ShaderType::FRAGMENT, std::string(defaultFrag.begin(), defaultFrag.end()));

    auto result = shader->compile();
    if (result.has_value()) {
        error() << result.value();
        throw std::runtime_error(result.value());
    }

    return shader;
}

std::shared_ptr<ShaderProgram> createMeshShader(Application* application, const std::string& name,
                                                const std::string& task, const std::string& mesh,
                                                const std::string& frag)
{
    auto shader = std::make_shared<ShaderProgram>(application, std::move(name));

    auto defaultMesh = cmrc::shaders::get_filesystem().open(mesh);
    auto defaultFrag = cmrc::shaders::get_filesystem().open(frag);

    if (!task.empty()) {
        auto defaultTask = cmrc::shaders::get_filesystem().open(task);
        shader->addShader(ShaderType::TASK, std::string(defaultTask.begin(), defaultTask.end()));
    }

    shader->addShader(ShaderType::MESH, std::string(defaultMesh.begin(), defaultMesh.end()));

    shader->addShader(ShaderType::FRAGMENT, std::string(defaultFrag.begin(), defaultFrag.end()));

    auto result = shader->compile();
    if (result.has_value()) {
        error() << result.value();
        throw std::runtime_error(result.value());
    }

    return shader;
}

std::shared_ptr<FrameBuffer> initRender(Room* room)
{
    auto* app = room->getApplication();

    // Bindings for the global uniforms.
    // In this application, we have a buffer of global parameters
    // and a skybox.
    std::vector<ShaderUniformBinding> globalBindings = {ShaderUniformBinding::uniformBuffer(sizeof(Matrices)),
                                                        ShaderUniformBinding::image()};

    // The description of the global uniforms.
    auto globalDescriptor = std::make_shared<ShaderUniformDescriptor>(app, "default", globalBindings);

    // The render of the application.
    // We should set the render to the application before
    // we do anything else, as some components depend on
    // the application's render.
    auto render = std::make_shared<Render>(app, "default", globalDescriptor);
    app->setRender(render);

    auto directionalShader = createShader(app, "directional_light", "directional_light.vert", "directional_light.frag");
    auto pointShader = createShader(app, "point_light", "point_light.vert", "point_light.frag");
    auto flashShader = createShader(app, "flash_light", "flash_light.vert", "flash_light.frag");
    auto screenShader = createShader(app, "screen", "screen.vert", "screen.frag");

    std::vector<FrameBufferTextureCreateInfo> frameBufferFormats = {
        TextureFormat::R8G8B8A8,
        TextureFormat::R16FG16F, // NORMAL XY
        TextureFormat::R16FG16F  // NORMAL Z / SPECULAR
    };

    auto fpFrameBuffer = std::make_shared<SimpleFrameBuffer>(app, "frame_buffer", frameBufferFormats, true);

    render->addRenderPass(std::make_shared<DefaultRenderPassStrategy>(fpFrameBuffer));

    auto outputs = fpFrameBuffer->getOutputs();
    std::vector<std::shared_ptr<Texture>> textures;
    textures.reserve(outputs.size());

    for (auto& output : outputs) {
        textures.push_back(output.resolvedTexture);
    }

    auto albedo = deferred_utils::createLightSystem(room, render.get(), textures, TextureFormat::R8G8B8A8,
                                                    directionalShader, pointShader, flashShader);

    std::vector<FrameBufferTextureCreateInfo> screenFormats = {TextureFormat::R8G8B8A8};
    auto screenFrameBuffer = std::make_shared<SimpleFrameBuffer>(app, "screen", screenFormats, false);
    render->addRenderPass(std::make_shared<DefaultRenderPassStrategy>(screenFrameBuffer));

    textures[0] = albedo;

    std::shared_ptr screenMaterial = Material::create(room->getApplication(), "Screen Model", screenFrameBuffer,
                                                      screenShader, deferred_utils::DeferredVertex::getDescription(),
                                                      InputDescription(0, InputRate::INSTANCE), {}, textures);

    auto screenModel = deferred_utils::createScreenModel(room->getApplication(), ModelCreateInfo(), "Screen Model");

    screenModel->addMaterial(screenMaterial);

    auto screenModelGO = room->newGameObject();
    screenModelGO->setName("Screen Model");
    screenModelGO->newComponent<GraphicComponent>(screenModel);

    auto swapFrameBuffer = std::make_shared<SwapChainFrameBuffer>(app, "swap_chain", SamplesPerTexel::COUNT_1, false);

    render->addRenderPass(std::make_shared<DefaultRenderPassStrategy>(swapFrameBuffer));

    return fpFrameBuffer;
}

void loadModels(Application* application, Room* room, const std::shared_ptr<FrameBuffer>& target)
{
    std::shared_ptr materialDescriptor = ShaderUniformDescriptor::ofImages(application, "default", 2);

    auto shader = createMeshShader(application, "deferred", "deferred.task", "deferred.mesh", "deferred.frag");

    auto modelLoaderInfo =
        assimp_loader::LoaderInfo::create<TestVertex>(application, "Zeppeli", MaterialCreateInfo(nullptr, nullptr));
    modelLoaderInfo.loadGPUModel = false;
    modelLoaderInfo.loadLocalModel = true;
    modelLoaderInfo.flipWindingOrder = false;
    modelLoaderInfo.flipNormals = false;

    TextureCreateInfo info;
    info.imageView.viewType = TextureViewType::NORMAL_2D;

    std::shared_ptr diffuse = Texture::createTextureFromFile(application, "Diffuse", "resource/Sans/diffuse.png", info);

    std::shared_ptr normal = Texture::createTextureFromFile(application, "Normal", "resource/Sans/normal.png", info);

    auto modelResult = load(R"(resource/Sans)", "Sans.obj", modelLoaderInfo);

    if (modelResult.error.has_value()) {
        error() << "Couldn't load model!";
        exit(1);
    }

    auto& localModel = modelResult.localModel;

    std::vector<TestVertex> vertices;

    for (auto& mesh : localModel->meshes) {
        for (uint32_t index : mesh.indices) {
            vertices.emplace_back(
                rush::Vec4f(mesh.vertices[index].position, 1.0f), rush::Vec4f(mesh.vertices[index].normal, 0.0f),
                rush::Vec4f(mesh.vertices[index].tangent, 1.0f), mesh.vertices[index].textureCoordinates);
        }
    }

    size_t sizeInBytes = vertices.size() * sizeof(TestVertex);

    std::vector<ShaderUniformBinding> cubeMaterialBindings;

    std::shared_ptr<ShaderUniformDescriptor> cubeMaterialDescriptor;
    materialDescriptor = std::make_shared<ShaderUniformDescriptor>(room->getApplication(), "pointMaterialDescriptor",
                                                                   cubeMaterialBindings);

    uint32_t instanceBufferSize = ModelCreateInfo::DEFAULT_MAXIMUM_INSTANCES * sizeof(DefaultInstancingData);

    std::vector modelBindings = {ShaderUniformBinding::storageBuffer(static_cast<uint32_t>(sizeInBytes)),
                                 ShaderUniformBinding::storageBuffer(instanceBufferSize),
                                 ShaderUniformBinding::uniformBuffer(sizeof(int)), ShaderUniformBinding::image(),
                                 ShaderUniformBinding::image()};

    auto modelDescriptor = std::make_shared<ShaderUniformDescriptor>(application, "model", modelBindings);

    MaterialCreateInfo cubeMaterialInfo(target, shader);
    cubeMaterialInfo.descriptions.uniform = materialDescriptor;
    cubeMaterialInfo.topology = PrimitiveTopology::POINT_LIST;
    cubeMaterialInfo.rasterizer.polygonMode = PolygonMode::FILL;
    cubeMaterialInfo.rasterizer.cullMode = CullMode::NONE;
    cubeMaterialInfo.descriptions.uniformBindings.insert({2, DescriptorBinding::extra(modelDescriptor)});

    auto material = std::make_shared<Material>(application, "pointMaterial", cubeMaterialInfo);

    ModelCreateInfo modelInfo;
    modelInfo.uniformDescriptor = modelDescriptor;

    modelInfo.instanceDataProvider = [](Application* app, const ModelCreateInfo& info, const Model* model) {
        std::vector indices = {StorageBufferInstanceData::Slot(
            sizeof(DefaultInstancingData), sizeof(DefaultInstancingData), 1, model->getUniformBuffer())};
        return std::vector<InstanceData*>{new StorageBufferInstanceData(app, info, indices)};
    };

    auto mesh = std::make_shared<MeshShaderDrawable>(application, "Mesh", material);

    mesh->setGroupsSupplier(
        [](const Model& model) { return rush::Vec<3, uint32_t>(1, model.getInstanceData(0)->getInstanceAmount(), 1); });

    modelInfo.drawables.push_back(mesh);

    auto model = std::make_shared<Model>(application, "Model", modelInfo);

    int verticesAmount = static_cast<int>(vertices.size());

    model->getUniformBuffer()->uploadData(0, vertices.data(), sizeInBytes);
    model->getUniformBuffer()->uploadData(2, verticesAmount);
    model->getUniformBuffer()->setTexture(3, diffuse);
    model->getUniformBuffer()->setTexture(4, normal);
    model->getUniformBuffer()->prepareForFrame(nullptr);

    for (int x = 0; x < 30; ++x) {
        for (int y = 0; y < 30; ++y) {
            auto object = room->newGameObject();
            object->newComponent<GraphicComponent>(model);
            object->newComponent<ConstantRotationComponent>();
            object->getTransform().setPosition(rush::Vec3f(x * 3, 0, y * 3));
            object->setName("Object");
        }
    }
}

std::shared_ptr<Texture> loadSkybox(Room* room)
{
    static const std::vector<std::string> PATHS = {
        "resource/Skybox/right.jpg",  "resource/Skybox/left.jpg",  "resource/Skybox/top.jpg",
        "resource/Skybox/bottom.jpg", "resource/Skybox/front.jpg", "resource/Skybox/back.jpg",
    };

    TextureCreateInfo info;
    info.imageView.viewType = TextureViewType::CUBE;
    info.image.layers = 6;

    return Texture::createTextureFromFiles(room->getApplication(), "skybox", PATHS, info);
}

std::shared_ptr<Room> getTestRoom(Application* application)
{
    auto room = std::make_shared<Room>(application);

    auto fpFrameBuffer = initRender(room.get());

    auto skybox = loadSkybox(room.get());
    application->getRender()->getGlobalUniformBuffer().setTexture(1, skybox);

    auto cameraController = room->newGameObject();
    auto cameraMovement = cameraController->newComponent<CameraMovementComponent>();
    cameraMovement->setSpeed(10.0f);

    auto parameterUpdater = room->newGameObject();
    parameterUpdater->newComponent<GlobalParametersUpdaterComponent>();
    parameterUpdater->newComponent<LockMouseComponent>(cameraMovement);
    parameterUpdater->newComponent<DockSpaceComponent>();
    parameterUpdater->newComponent<ViewportComponent>();
    auto goExplorer = parameterUpdater->newComponent<GameObjectExplorerComponent>();
    parameterUpdater->newComponent<SceneTreeComponent>(goExplorer);
    parameterUpdater->newComponent<DebugOverlayComponent>(false, 100);
    parameterUpdater->newComponent<LogComponent>();

#ifdef USE_VULKAN
    parameterUpdater->newComponent<vulkan::VulkanInfoCompontent>();
#endif

    auto directionalLight = room->newGameObject();
    directionalLight->newComponent<DirectionalLight>();
    directionalLight->getTransform().lookAt(rush::Vec3f(0.45f, -0.6f, 0.65f));
    directionalLight->setName("Directional light");

    auto pointLightGO = room->newGameObject();
    auto pointLight = pointLightGO->newComponent<PointLight>();
    pointLightGO->getTransform().setPosition({5.0f, 7.0f, 5.0f});
    pointLightGO->setName("Point light");
    pointLight->setDiffuseColor({1.0f, 0.0f, 0.0f});
    pointLight->setConstantAttenuation(0.01f);
    pointLight->setLinearAttenuation(0.2f);
    pointLight->setQuadraticAttenuation(0.1f);

    auto flashLightGO = room->newGameObject();
    auto flashLight = flashLightGO->newComponent<FlashLight>();
    flashLightGO->getTransform().setPosition({10.0f, 7.0f, 10.0f});
    flashLightGO->getTransform().rotate(rush::Vec3f(1.0f, 0.0f, 0.0f), 1.0f);
    flashLightGO->setName("Flash light");
    flashLight->setDiffuseColor({0.0f, 1.0f, 0.0f});
    flashLight->setConstantAttenuation(0.01f);
    flashLight->setLinearAttenuation(0.2f);
    flashLight->setQuadraticAttenuation(0.1f);

    loadModels(application, room.get(), fpFrameBuffer);

    return room;
}

int main()
{
    using MeshFeature = VkPhysicalDeviceMeshShaderFeaturesEXT;
    constexpr VkStructureType MESH_FEATURE = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;

    std::srand(std::time(nullptr));

    vulkan::VKApplicationCreateInfo info;
    info.name = "Neon";
    info.windowSize = {WIDTH, HEIGHT};
    info.vSync = false;
    info.defaultExtensionInclusion = vulkan::InclusionMode::EXCLUDE_ALL;
    info.defaultFeatureInclusion = vulkan::InclusionMode::EXCLUDE_ALL;

    info.extraFeatures.emplace_back(MeshFeature{MESH_FEATURE});

    info.featuresConfigurator = [](const auto& device, vulkan::VKPhysicalDeviceFeatures& features) {
        vulkan::VKApplicationCreateInfo::defaultFeaturesConfigurer(device, features);
        features.basicFeatures.geometryShader = true;
        features.basicFeatures.fillModeNonSolid = true;
        features.basicFeatures.samplerAnisotropy = true;

        auto* mesh = features.findFeature<MeshFeature>(MESH_FEATURE).value();
        mesh->meshShader = true;
        mesh->taskShader = true;

        features.extensions.emplace_back(VK_EXT_MESH_SHADER_EXTENSION_NAME);
    };

    info.deviceFilter = [](const vulkan::VKPhysicalDevice& device) {
        if (!vulkan::VKApplicationCreateInfo::defaultDeviceFilter(device)) {
            return false;
        }
        auto feature = device.getFeatures().findFeature<MeshFeature>(MESH_FEATURE);
        if (!feature.has_value()) {
            return false;
        }
        return feature.value()->meshShader > 0;
    };

    info.enableValidationLayers = true;

    Application application(std::make_unique<vulkan::VKApplication>(info));

    application.init();
    application.setRoom(getTestRoom(&application));

    auto loopResult = application.startGameLoop();
    if (loopResult.isOk()) {
        logger.done(MessageBuilder()
                        .print("Application closed. ")
                        .print(loopResult.getResult(), TextEffect::foreground4bits(2))
                        .print(" frames generated."));
    } else {
        logger.done(MessageBuilder()
                        .print("Unexpected game loop error: ")
                        .print(loopResult.getError(), TextEffect::foreground4bits(1)));
    }

    application.setRoom(nullptr);

    return EXIT_SUCCESS;
}
