//
// Created by grial on 19/10/22.
//

#include "GraphicComponent.h"

#include <engine/model/DefaultInstancingData.h>
#include <engine/structure/Room.h>

#include <imgui.h>

namespace neon {
    GraphicComponent::GraphicComponent() :
            _model(),
            _modelTargetId() {

    }

    GraphicComponent::GraphicComponent(IdentifiableWrapper<Model> model) :
            _model(model),
            _modelTargetId() {

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
        if (_model != nullptr && _modelTargetId.has_value()) {
            _model->freeInstance(*_modelTargetId.value());
        }
    }

    const IdentifiableWrapper<Model>& GraphicComponent::getModel() const {
        return _model;
    }

    void GraphicComponent::setModel(const IdentifiableWrapper<Model>& model) {
        if (_model != nullptr && _modelTargetId.has_value()) {
            _model->freeInstance(*_modelTargetId.value());
        }

        _model = model;

        if (_model != nullptr) {
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

        auto& models = getRoom()->getModels();

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

            for (auto& model: models) {
                if (!filter.PassFilter(model.getName().c_str())) continue;
                if (ImGui::Selectable(model.getName().c_str(),
                                      _model.getId() == model.getId())) {
                    setModel(&model);
                }
            }

            ImGui::EndPopup();
        }
    }
}
