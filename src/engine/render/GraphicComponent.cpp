//
// Created by grial on 19/10/22.
//

#include "GraphicComponent.h"

#include <engine/model/DefaultInstancingData.h>
#include <engine/structure/Room.h>
#include <engine/model/Model.h>
#include <engine/structure/collection/AssetCollection.h>

#include <imgui.h>

#include <utility>

namespace neon {
    GraphicComponent::GraphicComponent() :
            _model(),
            _modelTargetId(),
            _firstPreDrawExecuted(false) {

    }

    GraphicComponent::GraphicComponent(std::shared_ptr<Model> model) :
            _model(std::move(model)),
            _modelTargetId(),
            _firstPreDrawExecuted(false) {

        if (_model != nullptr) {
            auto result = _model->createInstance();
            if (result.isOk()) {
                _modelTargetId = result.getResult();
            } else {
                std::cerr << result.getError() << std::endl;
            }
        }
    }

    GraphicComponent::~GraphicComponent() {
        setModel(nullptr);
    }

    const std::shared_ptr<Model>& GraphicComponent::getModel() const {
        return _model;
    }

    void GraphicComponent::setModel(const std::shared_ptr<Model>& model) {
        if (_model != nullptr) {
            if (_modelTargetId.has_value()) {
                _model->freeInstance(*_modelTargetId.value());
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

            auto result = _model->createInstance();
            if (result.isOk()) {
                _modelTargetId = result.getResult();
            } else {
                std::cerr << result.getError() << std::endl;
                _modelTargetId = {};
            }
        }

    }

    void GraphicComponent::onPreDraw() {
        if (!_firstPreDrawExecuted) {
            if (_model != nullptr) {
                getRoom()->markUsingModel(_model.get());
            }
            _firstPreDrawExecuted = true;
        }

        if (!_modelTargetId.has_value()) return;
        if (_model->getInstancingStructType() != typeid(DefaultInstancingData))
            return;

        // Update default data!
        uploadData(DefaultInstancingData{
                getGameObject()->getTransform().getModel(),
                getGameObject()->getTransform().getNormal(),
        });
    }

    void GraphicComponent::drawEditor() {
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

            for (auto& [name, model]: models) {
                if (model.expired()) continue;
                if (!filter.PassFilter(name.c_str())) continue;
                if (ImGui::Selectable(name.c_str(),
                                      _model->getName() == name)) {
                    setModel(std::dynamic_pointer_cast<Model>(model.lock()));
                }
            }

            ImGui::EndPopup();
        }
    }
}
