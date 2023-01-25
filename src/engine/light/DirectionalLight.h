//
// Created by gaelr on 24/01/2023.
//

#ifndef NEON_DIRECTIONALLIGHT_H
#define NEON_DIRECTIONALLIGHT_H

#include <engine/structure/Component.h>
#include <engine/render/GraphicComponent.h>
#include <engine/model/Model.h>

class DirectionalLight : public Component {

    IdentifiableWrapper<GraphicComponent> _graphicComponent;
    IdentifiableWrapper<Model> _customModel;

    glm::vec3 _diffuseColor;
    glm::vec3 _specularColor;

public:

    struct Data {
        glm::vec3 diffuseColor;
        glm::vec3 specularColor;
        glm::vec3 direction;

        static InputDescription getDescription() {
            InputDescription description(
                    sizeof(Data),
                    InputRate::INSTANCE
            );

            description.addAttribute(3, 0);
            description.addAttribute(3, 12);
            description.addAttribute(3, 24);

            return description;
        }
    };

    DirectionalLight();

    explicit DirectionalLight(const IdentifiableWrapper<Model>& model);

    void onStart() override;

    void onLateUpdate(float deltaTime) override;

};


#endif //NEON_DIRECTIONALLIGHT_H
