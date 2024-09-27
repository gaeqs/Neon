//
// Created by gaelr on 20/11/2022.
//

#include "ConstantRotationComponent.h"

ConstantRotationComponent::ConstantRotationComponent() :
        _speed(static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)),
        _axis(0.0f, 1.0f, 0.0f) {
}

float ConstantRotationComponent::getSpeed() const {
    return _speed;
}

void ConstantRotationComponent::setSpeed(float speed) {
    _speed = speed;
}

const rush::Vec3f& ConstantRotationComponent::getAxis() const {
    return _axis;
}

void ConstantRotationComponent::setAxis(const rush::Vec3f& axis) {
    _axis = axis;
}

void ConstantRotationComponent::onUpdate(float deltaTime) {
    getGameObject()->getTransform().rotate(
            _axis,
            deltaTime * _speed
    );
}

void ConstantRotationComponent::drawEditor() {
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Speed (rad/s):");
    ImGui::SameLine();
    ImGui::DragFloat(imGuiUId("##speed").c_str(), &_speed, 0.05f);

    ImGui::PushItemWidth(-1.0f);
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Axis:");
    ImGui::SameLine();
    ImGui::DragFloat3(imGuiUId("##axis").c_str(), &_axis.x(), 0.1f);
    ImGui::PopItemWidth();
}
