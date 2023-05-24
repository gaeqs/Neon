//
// Created by gaelr on 04/11/2022.
//

#ifndef NEON_GLOBALPARAMETERSUPDATERCOMPONENT_H
#define NEON_GLOBALPARAMETERSUPDATERCOMPONENT_H

#include <engine/Engine.h>

struct GlobalParameters {
    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 projectionView;
    glm::mat4 inverseProjection;
    glm::vec2 screenSize;
    float near;
    float far;
    float metallic;
    float roughness;
};

class GlobalParametersUpdaterComponent : public neon::Component {

    float metallic = 0.0f;
    float roughness = 0.0f;

    bool useSSAO = true;

public:

    ~GlobalParametersUpdaterComponent() override;

    void onStart() override;

    void onUpdate(float deltaTime) override;

    void drawEditor() override;

    void onKey(const neon::KeyboardEvent &event) override;

};
REGISTER_COMPONENT(GlobalParametersUpdaterComponent,
                   "Global Parameters Updater")


#endif //NEON_GLOBALPARAMETERSUPDATERCOMPONENT_H
