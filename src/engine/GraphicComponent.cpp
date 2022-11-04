//
// Created by grial on 19/10/22.
//

#include "GraphicComponent.h"

#include <engine/model/DefaultInstancingData.h>

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

void GraphicComponent::onUpdate(float deltaTime) {
    if (!_modelTargetId.has_value()) return;
    if (_model->getInstancingStructType() != typeid(DefaultInstancingData))
        return;

    // Update default data!
    uploadData(DefaultInstancingData{
            getGameObject()->getTransform().getModel()
    });
}
