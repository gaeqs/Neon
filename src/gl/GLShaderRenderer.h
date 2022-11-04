//
// Created by gaelr on 23/10/2022.
//

#ifndef RVTRACKING_GLSHADERRENDERER_H
#define RVTRACKING_GLSHADERRENDERER_H

#include <string>
#include <memory>

#include <engine/Renderer.h>
#include <gl/Shader.h>

class GraphicComponent;

class GLShaderRenderer : public Renderer {

    std::unordered_map<std::string, std::shared_ptr<Shader>> _shaders;

public:

    GLShaderRenderer();

    void insertShader(const std::string& name,
                      std::shared_ptr<Shader> shader);

    virtual void preRenderConfiguration();

    void render(Room* room, std::shared_ptr<ComponentList> elements) override;

};


#endif //RVTRACKING_GLSHADERRENDERER_H
