//
// Created by gaelr on 23/10/2022.
//

#ifndef RVTRACKING_SHADERCONTROLLER_H
#define RVTRACKING_SHADERCONTROLLER_H

#include <memory>

#include <engine/IdentifiableWrapper.h>
#include <engine/GraphicComponent.h>
#include <gl/Shader.h>


class Room;

class ShaderController {

    std::shared_ptr<Shader> _shader;

public:

    explicit ShaderController(std::shared_ptr<Shader> shader);

    const std::shared_ptr<Shader>& getShader() const;

    virtual void setupGlobalUniforms(Room* room) = 0;

    virtual void setupGraphicComponentUniforms(GraphicComponent* component) = 0;

};


#endif //RVTRACKING_SHADERCONTROLLER_H
