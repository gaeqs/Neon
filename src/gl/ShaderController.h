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

    /**
     * This method uploads all global uniforms required by the shader.
     * @param room the current room.
     */
    virtual void setupAdditionalGlobalUniforms(Room* room) = 0;

    /**
     * This method uploads all additional uniforms of a GraphicComponent
     * required by the shader.
     *
     * Remember, material's uniforms are already uploaded!
     *
     * @param component the component.
     */
    virtual void setupAdditionalGraphicComponentUniforms(
            GraphicComponent* component) = 0;

};


#endif //RVTRACKING_SHADERCONTROLLER_H
