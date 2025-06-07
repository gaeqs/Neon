//
// Created by gaeqs on 6/21/24.
//

#ifndef DEBUGRENDERCOMPONENT_H
#define DEBUGRENDERCOMPONENT_H

#include <neon/Neon.h>

#include <utility>

CMRC_DECLARE(shaders);

class DebugRenderComponent : public neon::Component
{
    struct Instance
    {
        rush::Vec3f position;
        rush::Vec4f color;
        float radius;

        static neon::InputDescription getInstancingDescription()
        {
            neon::InputDescription description(sizeof(Instance), neon::InputRate::INSTANCE);
            description.addAttribute(3, 0);
            description.addAttribute(4, 12);
            description.addAttribute(1, 28);

            return description;
        }
    };

    std::shared_ptr<neon::FrameBuffer> _target;
    std::shared_ptr<neon::Model> _model;
    std::vector<neon::InstanceData::Instance> _frontInstances;
    std::vector<neon::InstanceData::Instance> _backInstances;

    std::shared_ptr<neon::Material> createMaterial(const neon::Room* room) const
    {
        auto program = std::make_shared<neon::ShaderProgram>(room->getApplication(), "debug");

        program->addShader(neon::ShaderType::VERTEX, cmrc::shaders::get_filesystem().open("debug.vert"));

        program->addShader(neon::ShaderType::FRAGMENT, cmrc::shaders::get_filesystem().open("debug.frag"));

        if (auto result = program->compile(); result.has_value()) {
            std::cerr << result.value() << std::endl;
            throw std::runtime_error(result.value());
        }

        neon::MaterialCreateInfo info(_target, program);

        neon::InputDescription desc(sizeof(float), neon::InputRate::VERTEX);
        desc.addAttribute(1, 0);

        info.descriptions.vertex.push_back(desc);
        info.descriptions.instance.push_back(Instance::getInstancingDescription());

        return std::make_shared<neon::Material>(room->getApplication(), "debug", info);
    }

    std::shared_ptr<neon::Model> createModel(const neon::Room* room) const
    {
        auto material = createMaterial(room);

        neon::ModelCreateInfo info;
        info.maximumInstances = 2000000;

        std::vector<float> vertices{0.0f, 1.0f, 2.0f, 3.0f};

        std::vector<uint32_t> indices{0, 1, 2, 1, 3, 2};

        auto mesh = std::make_unique<neon::Mesh>(room->getApplication(), "debug", material, false, false);

        mesh->uploadVertices(vertices);
        mesh->uploadIndices(indices);

        info.drawables.push_back(std::move(mesh));

        info.defineInstanceType<Instance>();

        auto model = std::make_shared<neon::Model>(room->getApplication(), "debug", info);

        return model;
    }

  public:
    explicit DebugRenderComponent(std::shared_ptr<neon::FrameBuffer> target, neon::Room* room) :
        _target(std::move(target))
    {
        _model = createModel(room);
        room->markUsingModel(_model.get());
    }

    ~DebugRenderComponent() override
    {
        getRoom()->unmarkUsingModel(_model.get());
    }

    void drawElement(const rush::Vec3f& position, const rush::Vec4f& color, float radius)
    {
        neon::InstanceData::Instance instance;
        if (!_frontInstances.empty()) {
            instance = _frontInstances.back();
            _frontInstances.pop_back();
        } else {
            auto result = _model->getInstanceData(0)->createInstance();
            if (!result.isOk()) {
                std::cerr << result.getError() << std::endl;
                return;
            }
            instance = result.getResult();
        }

        _backInstances.push_back(instance);
        _model->getInstanceData(0)->uploadData<Instance>(instance, 0, Instance(position, color, radius));
    }

    void drawPermanent(const rush::Vec3f& position, const rush::Vec4f& color, float radius)
    {
        neon::InstanceData::Instance instance;
        if (!_frontInstances.empty()) {
            instance = _frontInstances.back();
            _frontInstances.pop_back();
        } else {
            auto result = _model->getInstanceData(0)->createInstance();
            if (!result.isOk()) {
                std::cerr << result.getError() << std::endl;
                return;
            }
            instance = result.getResult();
        }
        _model->getInstanceData(0)->uploadData<Instance>(instance, 0, Instance(position, color, radius));
    }

    void onPreDraw() override
    {
        std::swap(_frontInstances, _backInstances);
        auto* instanceData = _model->getInstanceData(0);
        for (const neon::InstanceData::Instance& instance : _backInstances) {
            instanceData->freeInstance(instance);
        }
        _backInstances.clear();
    }
};

#endif // DEBUGRENDERCOMPONENT_H
