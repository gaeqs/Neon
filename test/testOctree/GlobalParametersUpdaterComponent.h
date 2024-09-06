//
// Created by gaelr on 04/11/2022.
//

#ifndef NEON_GLOBALPARAMETERSUPDATERCOMPONENT_H
#define NEON_GLOBALPARAMETERSUPDATERCOMPONENT_H

#include <neon/Neon.h>

struct Matrices {
    rush::Mat4f view;
    rush::Mat4f viewProjection;
    rush::Mat4f alignedViewProjection;
    rush::Mat4f inverseProjection;
    float near;
    float far;
};

struct Timestamp {
    float time;
};

class GlobalParametersUpdaterComponent : public neon::Component {

    float _timestamp = 0.0f;

public:

    ~GlobalParametersUpdaterComponent() override;

    void onStart() override;

    void onUpdate(float deltaTime) override;

    void drawEditor() override;

};
REGISTER_COMPONENT(GlobalParametersUpdaterComponent,
                   "Global Parameters Updater")


#endif //NEON_GLOBALPARAMETERSUPDATERCOMPONENT_H
