//
// Created by gaelr on 23/10/2022.
//

#ifndef RVTRACKING_GLSHADERRENDERER_H
#define RVTRACKING_GLSHADERRENDERER_H

#include <string>
#include <memory>

#include <engine/Renderer.h>
#include <gl/ShaderController.h>

class GLShaderRenderer : public Renderer {

    std::unordered_map<std::string, std::shared_ptr<ShaderController>> _shaders;

protected:


    virtual void uploadGlobalUniforms(
            const std::shared_ptr<Shader>& shader, Room* room);

    virtual void uploadGraphicObjectUniforms(
            const std::shared_ptr<Shader>& shader, GraphicComponent* component);

public:

    GLShaderRenderer();

    void insertShader(const std::string& name,
                      std::shared_ptr<ShaderController> shader);

    virtual void preRenderConfiguration();

    void render(Room* room, std::shared_ptr<ComponentList> elements) override;

};


#endif //RVTRACKING_GLSHADERRENDERER_H
