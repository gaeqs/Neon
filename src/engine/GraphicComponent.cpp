//
// Created by grial on 19/10/22.
//

#include "GraphicComponent.h"

GraphicComponent::GraphicComponent() : _material(), _model() {

}

const Material& GraphicComponent::getMaterial() const {
    return _material;
}

Material& GraphicComponent::getMaterial() {
    return _material;
}

void GraphicComponent::setMaterial(const Material& material) {
    _material = material;
}

const IdentifiableWrapper<Model>& GraphicComponent::getModel() const {
    return _model;
}

IdentifiableWrapper<Model>& GraphicComponent::getModel() {
    return _model;
}

void GraphicComponent::setModel(const IdentifiableWrapper<Model>& model) {
    _model = model;
}
