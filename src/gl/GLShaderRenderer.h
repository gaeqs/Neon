//
// Created by gaelr on 23/10/2022.
//

#ifndef RVTRACKING_GLSHADERRENDERER_H
#define RVTRACKING_GLSHADERRENDERER_H

#include <engine/Renderer.h>
#include <gl/ShaderController.h>

class GLShaderRenderer : public Renderer {

    std::shared_ptr<ShaderController> _shader;

public:

    explicit GLShaderRenderer(std::shared_ptr<ShaderController> shader);

    void render(Room* room, std::shared_ptr<ComponentList> elements) override;

};


#endif //RVTRACKING_GLSHADERRENDERER_H
