//
// Created by grial on 19/10/22.
//

#include "GraphicComponent.h"

#include <neon/render/model/DefaultInstancingData.h>
#include <neon/structure/Room.h>
#include <neon/render/model/Model.h>
#include <neon/structure/collection/AssetCollection.h>

#include <imgui.h>

#include <utility>

namespace neon
{
    GraphicComponent::GraphicComponent() :
        _firstPreDrawExecuted(false)
    {
    }

    GraphicComponent::GraphicComponent(std::shared_ptr<Model> model) :
        _model(std::move(model)),
        _firstPreDrawExecuted(false)
    {
        if (_model != nullptr) {
            auto result = _model->getInstanceData(0)->createInstance();
            if (result.isOk()) {
                _modelTargetId = result.getResult();
            } else {
                error(result.getError());
            }
        }
    }

    GraphicComponent::~GraphicComponent()
    {
        setModel(nullptr);
    }

    const std::shared_ptr<Model>& GraphicComponent::getModel() const
    {
        return _model;
    }

    void GraphicComponent::setModel(const std::shared_ptr<Model>& model)
    {
        if (model == _model) {
            return;
        }
        if (_model != nullptr) {
            if (_modelTargetId.has_value()) {
                _model->getInstanceData(0)->freeInstance(_modelTargetId.value());
            }
            if (_firstPreDrawExecuted) {
                getRoom()->unmarkUsingModel(model.get());
            }
        }

        _model = model;

        if (_model != nullptr) {
            if (_firstPreDrawExecuted) {
                getRoom()->markUsingModel(model.get());
            }

            auto result = _model->getInstanceData(0)->createInstance();
            if (result.isOk()) {
                _modelTargetId = result.getResult();
            } else {
                getLogger().error(
                    MessageBuilder().print("Error creating a render instance: ").print(result.getError()));
                _modelTargetId = {};
            }
        }
    }

    void GraphicComponent::onPreDraw()
    {
        if (!_firstPreDrawExecuted) {
            if (_model != nullptr) {
                getRoom()->markUsingModel(_model.get());
            }
            _firstPreDrawExecuted = true;
        }

        if (!_modelTargetId.has_value()) {
            return;
        }

        auto& types = _model->getInstanceData(0)->getInstancingStructTypes();

        const DefaultInstancingData data{
            getGameObject()->getTransform().getModel(),
            getGameObject()->getTransform().getNormal(),
        };

        for (size_t i = 0; i < types.size(); ++i) {
            if (types[i] == typeid(DefaultInstancingData)) {
                uploadData(i, data);
            }
        }
    }

    void GraphicComponent::drawEditor()
    {
        static ImGuiTextFilter filter;

        auto& models = getAssets().getAll<Model>();

        if (_model) {
            ImGui::Text("Using model %s", _model->getName().c_str());
        } else {
            ImGui::Text("This GraphicComponent is not using any model.");
        }

        if (ImGui::Button("Change model", ImVec2(-1, 0))) {
            ImGui::OpenPopup("model_change_popup");
            filter.Clear();
        }

        if (ImGui::BeginPopup("model_change_popup")) {
            filter.Draw();

            for (auto& [name, model] : models) {
                if (model.expired()) {
                    continue;
                }
                if (!filter.PassFilter(name.c_str())) {
                    continue;
                }
                if (ImGui::Selectable(name.c_str(), _model->getName() == name)) {
                    setModel(std::dynamic_pointer_cast<Model>(model.lock()));
                }
            }

            ImGui::EndPopup();
        }
    }
} // namespace neon
