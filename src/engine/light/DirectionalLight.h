//
// Created by gaelr on 24/01/2023.
//

#ifndef NEON_DIRECTIONALLIGHT_H
#define NEON_DIRECTIONALLIGHT_H

#include <engine/structure/Component.h>
#include <engine/render/GraphicComponent.h>
#include <engine/model/Model.h>

class DirectionalLight : Component {

    IdentifiableWrapper<GraphicComponent> _graphicComponent;

    glm::vec3 _diffuseColor;
    glm::vec3 _specularColor;
public:

    struct Data {
        glm::vec3 diffuseColor;
        glm::vec3 specularColor;
        glm::vec3 direction;
    };

    explicit DirectionalLight(const IdentifiableWrapper<Model>& model);

    void onLateUpdate(float deltaTime) override;

};


#endif //NEON_DIRECTIONALLIGHT_H
