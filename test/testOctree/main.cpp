#include <iostream>
#include <memory>
#include <vector>
#include <random>

#include <neon/Neon.h>

#include <neon/util/component/CameraMovementComponent.h>
#include <neon/util/component/DebugOverlayComponent.h>
#include <neon/util/component/DockSpaceComponent.h>
#include <neon/util/component/ViewportComponent.h>
#include <neon/util/component/SceneTreeComponent.h>
#include <neon/util/component/GameObjectExplorerComponent.h>
#include <neon/util/DeferredUtils.h>

#include <neon/assimp/AssimpLoader.h>

#include "DebugRenderComponent.h"
#include "TestVertex.h"
#include "GlobalParametersUpdaterComponent.h"
#include "LockMouseComponent.h"
#include "ObjectRaycastView.h"
#include "OctreeRaycastView.h"
#include "OctreeView.h"

constexpr int32_t WIDTH = 800;
constexpr int32_t HEIGHT = 600;

using namespace neon;

CMRC_DECLARE(shaders);

std::vector<rush::TreeContent<size_t, rush::Sphere<3, float>>>
generateDummyElements() {
    std::random_device os_seed;
    uint32_t seed = os_seed();
    std::default_random_engine g(seed);
    std::uniform_real_distribution d(-10.0f, 10.0f);

    auto rVec = [&g, &d]() {
        return rush::Vec3f(d(g), d(g), d(g));
    };

    std::vector<rush::TreeContent<size_t, rush::Sphere<3, float>>> content;
    for (size_t i = 0; i < 1000; ++i) {
        auto vec = rVec().normalized() * 5.0f;

        content.push_back({
            rush::Sphere(vec, 0.1f),
            i
        });
    }

    return content;
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
    auto defaultVert = cmrc::shaders::get_filesystem().open(vert);
    auto defaultFrag = cmrc::shaders::get_filesystem().open(frag);

    auto shader = std::make_shared<ShaderProgram>(application, name);
    shader->addShader(ShaderType::VERTEX, defaultVert);
    shader->addShader(ShaderType::FRAGMENT, defaultFrag);

    auto result = shader->compile();
    if (result.has_value()) {
        std::cerr << result.value() << std::endl;
        throw std::runtime_error(result.value());
    }

    return shader;
}

std::shared_ptr<FrameBuffer> initRender(Room* room) {
    auto* app = room->getApplication();

    // Bindings for the global uniforms.
    // In this application, we have a buffer of global parameters
    // and a skybox.
    std::vector<ShaderUniformBinding> globalBindings = {
        {UniformBindingType::UNIFORM_BUFFER, sizeof(Matrices)},
        {UniformBindingType::UNIFORM_BUFFER, sizeof(Timestamp)},
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

    // The format of the first frame buffer.
    std::vector<FrameBufferTextureCreateInfo> frameBufferFormats = {
        TextureFormat::R8G8B8A8,
        TextureFormat::R16FG16F, // NORMAL XY
        TextureFormat::R16FG16F // NORMAL Z / SPECULAR
    };

    // Here we create the first frame buffer.
    // Just after creation, the frame buffer should be added
    // to the render as a render pass.
    // We'll use the default strategy for the rendering.
    auto fpFrameBuffer = std::make_shared<SimpleFrameBuffer>(
        room->getApplication(), frameBufferFormats, true);

    render->addRenderPass(std::make_shared<DefaultRenderPassStrategy>(
        fpFrameBuffer));

    // Here we create the second frame buffer.
    // Just like the first frame buffer, we define the output,
    // create the frame buffer and add a render pass.
    std::vector<FrameBufferTextureCreateInfo> screenFormats = {
        TextureFormat::R8G8B8A8
    };
    auto screenFrameBuffer = std::make_shared<SimpleFrameBuffer>(
        room->getApplication(), screenFormats, false);
    render->addRenderPass(std::make_shared<DefaultRenderPassStrategy>(
        screenFrameBuffer));


    // Here we create a model that renders the screen on
    // the second render pass.
    // This model is a screen plane that uses the
    // output textures of the first render pass.

    auto screenShader = createShader(
        app, "screen", "screen.vert", "screen.frag");

    auto textures = fpFrameBuffer->getTextures();

    // We can also create a GraphicComponent that handles
    // an instance of the model.
    // This option is more direct.
    auto screenModel = deferred_utils::createScreenModel(
        app,
        ModelCreateInfo(),
        "screen model"
    );

    std::shared_ptr<Material> screenMaterial = Material::create(
        room->getApplication(), "Screen Model",
        screenFrameBuffer, screenShader,
        deferred_utils::DeferredVertex::getDescription(),
        InputDescription(0, InputRate::INSTANCE),
        {}, textures);
    screenModel->addMaterial(screenMaterial);

    auto screenModelGO = room->newGameObject();
    screenModelGO->setName("Screen Model");
    screenModelGO->newComponent<GraphicComponent>(screenModel);


    // Finally, we create the swap chain buffer.
    // We split the screen render in two frame buffers
    // because of ImGUI.
    // ImGUI will use the result of screenFrameBuffer
    // to render a viewport.
    // Then, it will render its UI.
    // If you don't use ImGUI, you don't have to
    // split the screen render in two frame buffers.
    auto swapFrameBuffer = std::make_shared<SwapChainFrameBuffer>(room, false);
    render->addRenderPass(std::make_shared<DefaultRenderPassStrategy>(
        swapFrameBuffer));

    return fpFrameBuffer;
}

void loadModels(Application* application, Room* room,
                const std::shared_ptr<FrameBuffer>& target,
                const std::shared_ptr<Material>& lineMaterial) {
    std::shared_ptr materialDescriptor =
            ShaderUniformDescriptor::ofImages(application,
                                              "default", 2);

    auto shader = createShader(application,
                               "model",
                               "model.vert", "model.frag");

    MaterialCreateInfo sansMaterialInfo(target, shader);
    sansMaterialInfo.descriptions.uniform = materialDescriptor;

    auto sansLoaderInfo = assimp_loader::LoaderInfo::create<ModelVertex>(
        application, "Sans", sansMaterialInfo);
    sansLoaderInfo.loadLocalModel = true;

    auto sansResult = assimp_loader::load(R"(resource/Sans)",
                                          "Sans.obj", sansLoaderInfo);

    if (sansResult.error.has_value()) {
        std::cout << "Couldn't load Sans model!" << std::endl;
        exit(1);
    }

    auto sansModel = sansResult.model;

    auto sans = room->newGameObject();
    sans->newComponent<GraphicComponent>(sansModel);

    rush::Vec3f t = rush::Vec3f(20, 0, 0);

    sans->getTransform().setPosition(t);
    sans->setName("Sans");


    // Get triangles

    auto& sansLoadModel = sansResult.localModel;

    std::vector<rush::Triangle<float>> triangles;
    triangles.reserve(10000);

    for (auto& mesh: sansLoadModel->meshes) {
        for (size_t n = 0; n < mesh.indices.size(); n += 3) {
            triangles.emplace_back(
                mesh.vertices[mesh.indices[n]].position + t,
                mesh.vertices[mesh.indices[n + 1]].position + t,
                mesh.vertices[mesh.indices[n + 2]].position + t
            );
        }
    }

    std::cout << "AMOUNT: " << triangles.size() << std::endl;

    // Generate sans tree

    rush::AABB<3, float> treeBounds = {
        {20.0f, 0.0f, 0.0f}, rush::Vec3f(5.0f)
    };

    std::vector<rush::TreeContent<size_t, rush::Triangle<float>>> contents;
    contents.reserve(triangles.size());

    for (size_t i = 0; i < triangles.size(); ++i) {
        contents.emplace_back(triangles[i], i);
    }

    rush::StaticTree<size_t, rush::Triangle<float>, 3, float, 10, 2> tree(
        treeBounds, contents);


    auto box = room->newGameObject();
    box->setName("Octree Raycast View");
    auto debug = box->newComponent<DebugRenderComponent>(
        target, room);

    box->newComponent<OctreeRaycastView<size_t, rush::Triangle<float>, 10, 2>>(
        debug,
        lineMaterial,
        std::move(tree));

    rush::Triangle<float> tri(
        {-10.0f, 0.0f, 0.0f},
        {-10.0f, 0.0f, 2.0f},
        {-8.0f, 1.0f, 1.0f}
    );

    for (float f = 0.0f; f <= 1.0f; f += 0.01f) {
        debug->drawPermanent(
            tri.a * f + tri.b * (1.0f - f),
            rush::Vec4f(1.0f, 0.0f, 0.0f, 1.0f),
            0.01f
        );
        debug->drawPermanent(
            tri.b * f + tri.c * (1.0f - f),
            rush::Vec4f(1.0f, 0.0f, 0.0f, 1.0f),
            0.01f
        );
        debug->drawPermanent(
            tri.c * f + tri.a * (1.0f - f),
            rush::Vec4f(1.0f, 0.0f, 0.0f, 1.0f),
            0.01f
        );
    }

    box->newComponent<ObjectRaycastView<rush::Triangle<float>>>(
        debug,
        tri
    );
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

    return Texture::createTextureFromFiles(
        room->getApplication(),
        "skybox",
        PATHS,
        info
    );
}

std::shared_ptr<Room> getTestRoom(Application* application) {
    auto room = std::make_shared<Room>(application);
    auto fpFrameBuffer = initRender(room.get());

    auto skybox = loadSkybox(room.get());
    room->getApplication()->getRender()->
            getGlobalUniformBuffer().setTexture(2, skybox);


    auto cameraController = room->newGameObject();
    cameraController->newComponent<GlobalParametersUpdaterComponent>();
    auto cameraMovement = cameraController->newComponent<
        CameraMovementComponent>();
    cameraMovement->setSpeed(10.0f);

    auto imgui = room->newGameObject();
    imgui->newComponent<LockMouseComponent>(cameraMovement);
    imgui->newComponent<DockSpaceComponent>();
    imgui->newComponent<ViewportComponent>();
    auto goExplorer = imgui->newComponent<GameObjectExplorerComponent>();
    imgui->newComponent<SceneTreeComponent>(goExplorer);
    imgui->newComponent<DebugOverlayComponent>(false, 100);


    std::shared_ptr<ShaderUniformDescriptor> materialDescriptor =
            ShaderUniformDescriptor::ofImages(application, "descriptor", 1);

    auto shader = createShader(application,
                               "deferred", "deferred.vert", "deferred.frag");

    MaterialCreateInfo lineMaterialInfo(fpFrameBuffer, shader);
    lineMaterialInfo.descriptions.uniform = materialDescriptor;
    lineMaterialInfo.descriptions.instance.push_back(
        DefaultInstancingData::getInstancingDescription());
    lineMaterialInfo.descriptions.vertex.
            push_back(TestVertex::getDescription());
    lineMaterialInfo.rasterizer.polygonMode = neon::PolygonMode::FILL;
    lineMaterialInfo.rasterizer.cullMode = neon::CullMode::NONE;
    lineMaterialInfo.rasterizer.lineWidth = 5.0f;

    lineMaterialInfo.topology = neon::PrimitiveTopology::LINE_LIST;

    auto material = std::make_shared<Material>(application, "line",
                                               lineMaterialInfo);

    auto box = room->newGameObject();
    box->setName("Box");
    auto debug = box->newComponent<DebugRenderComponent>(
        fpFrameBuffer, room.get());


    rush::AABB<3, float> treeBounds = {
        {0.0f, 0.0f, 0.0f}, rush::Vec3f(10.0f)
    };

    auto content = generateDummyElements();

    rush::StaticTree<size_t, rush::Sphere<3, float>, 3, float, 10, 2> tree(
        treeBounds, content);

    for (const auto& c: content) {
        debug->drawPermanent(
            c.bounds.center,
            {0.0f, 1.0f, 0.0f, 1.0f},
            c.bounds.radius
        );
    }


    box->newComponent<OctreeRaycastView<size_t, rush::Sphere<3, float>, 10, 2>>(
        debug,
        material,
        std::move(tree));

    room->getCamera().lookAt({0, 1.0f, -1.0f});
    room->getCamera().setPosition({0.0f, 3.0f, 3.0f});

    loadModels(application, room.get(), fpFrameBuffer, material);

    return room;
}

int main() {
    std::srand(std::time(nullptr));

    vulkan::VKApplicationCreateInfo info;
    info.name = "Neon";
    info.windowSize = {WIDTH, HEIGHT};

    info.featuresConfigurator = [](const auto& d, auto& f) {
        vulkan::VKApplicationCreateInfo::defaultFeaturesConfigurer(d, f);
        f.basicFeatures.samplerAnisotropy = true;
        f.basicFeatures.wideLines = true;
    };

    Application application(std::make_unique<vulkan::VKApplication>(info));

    application.init();
    application.setRoom(getTestRoom(&application));

    auto loopResult = application.startGameLoop();
    if (loopResult.isOk()) {
        std::cout << "[APPLICATION]\tApplication closed. "
                << loopResult.getResult() << " frames generated."
                << std::endl;
    }
    else {
        std::cout << "[APPLICATION]\tUnexpected game loop error: "
                << loopResult.getError()
                << std::endl;
    }

    application.setRoom(nullptr);

    return EXIT_SUCCESS;
}
