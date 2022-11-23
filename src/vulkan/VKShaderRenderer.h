//
// Created by gaelr on 16/11/2022.
//

#ifndef NEON_VKSHADERRENDERER_H
#define NEON_VKSHADERRENDERER_H

#include <engine/Renderer.h>
#include <engine/IdentifiableWrapper.h>
#include <engine/shader/ShaderProgram.h>

class Application;

class VKApplication;

class VKShaderRenderer : public Renderer {

    VKApplication* _vkApplication;
    std::unordered_map<std::string, IdentifiableWrapper<ShaderProgram>> _shaders;

public:

    explicit VKShaderRenderer(Application* application);

    void insertShader(const std::string& name,
                      IdentifiableWrapper<ShaderProgram> shader);

    void render(Room* room, std::shared_ptr<ComponentList> elements) override;

};


#endif //NEON_VKSHADERRENDERER_H
