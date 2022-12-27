//
// Created by gaelr on 04/11/2022.
//

#ifndef NEON_GLOBALPARAMETERSUPDATERCOMPONENT_H
#define NEON_GLOBALPARAMETERSUPDATERCOMPONENT_H

#include <engine/Engine.h>

struct GlobalParameters {
    glm::mat4 view;
    glm::mat4 projectionView;
};

class GlobalParametersUpdaterComponent : public Component {

public:

    ~GlobalParametersUpdaterComponent() override;

    void onStart() override;

    void onUpdate(float deltaTime) override;

};


#endif //NEON_GLOBALPARAMETERSUPDATERCOMPONENT_H
