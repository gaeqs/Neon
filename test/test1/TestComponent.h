//
// Created by gaelr on 20/10/2022.
//

#ifndef RVTRACKING_TESTCOMPONENT_H
#define RVTRACKING_TESTCOMPONENT_H


#include <engine/Engine.h>
#include <glm/glm.hpp>

class TestComponent : public Component {

    IdentifiableWrapper<GraphicComponent> _graphic;

public:

    ~TestComponent() override;

    void onConstruction() override;

    void onStart() override;

    void onUpdate(float deltaTime) override;

};


#endif //RVTRACKING_TESTCOMPONENT_H
