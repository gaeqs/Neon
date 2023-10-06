//
// Created by gaelr on 04/11/2022.
//

#ifndef NEON_GLOBALPARAMETERSUPDATERCOMPONENT_H
#define NEON_GLOBALPARAMETERSUPDATERCOMPONENT_H

#include <rush/rush.h>

#include <engine/Engine.h>

struct Matrices {
    rush::Mat4f view;
    rush::Mat4f projection;
    rush::Mat4f projectionView;
    rush::Mat4f inverseProjection;
    rush::Vec2f screenSize;
    float near;
    float far;
};

struct PBRParameters {
    float metallic;
    float roughness;
    uint32_t useSSAO;
    uint32_t shoOnlySSAO;
    uint32_t ssaoFilterRadius;
    float skyboxLod;
    float bloomIntensity;
    float bloomFilterRadius;
};

class GlobalParametersUpdaterComponent : public neon::Component {

    PBRParameters _pbr = {0.4f, 0.4f, true, false, 2, 0.0f, 0.04f, 0.01f};

public:

    ~GlobalParametersUpdaterComponent() override;

    void onStart() override;

    void onUpdate(float deltaTime) override;

    void drawEditor() override;

    void onKey(const neon::KeyboardEvent& event) override;

};
REGISTER_COMPONENT(GlobalParametersUpdaterComponent,
                   "Global Parameters Updater")


#endif //NEON_GLOBALPARAMETERSUPDATERCOMPONENT_H
