//
// Created by gaelr on 24/10/2022.
//

#ifndef RVTRACKING_TESTSHADERCONTROLLER_H
#define RVTRACKING_TESTSHADERCONTROLLER_H

#include "engine/Engine.h"
#include "gl/ShaderController.h"
#include "gl/Shader.h"

class TestShaderController : public ShaderController {

public:

    explicit TestShaderController(const std::shared_ptr<Shader>& shader);

    void setupAdditionalGlobalUniforms(Room* room) override;

    void setupAdditionalGraphicComponentUniforms(GraphicComponent* component) override;
};


#endif //RVTRACKING_TESTSHADERCONTROLLER_H