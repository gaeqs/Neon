//
// Created by gaelr on 24/01/2023.
//

#ifndef NEON_POINTLIGHT_H
#define NEON_POINTLIGHT_H

#include <engine/structure/Component.h>
#include <engine/render/GraphicComponent.h>
#include <engine/model/Model.h>

namespace neon {

    class PointLight : public Component {

        IdentifiableWrapper<GraphicComponent> _graphicComponent;
        std::shared_ptr<Model> _customModel;

        glm::vec3 _diffuseColor;
        glm::vec3 _specularColor;
        float _constantAttenuation;
        float _linearAttenuation;
        float _quadraticAttenuation;

    public:

        struct Data {
            glm::vec3 diffuseColor;
            glm::vec3 specularColor;
            glm::vec3 position;
            float constantAttenuation;
            float linearAttenuation;
            float quadraticAttenuation;

            static InputDescription getDescription() {
                InputDescription description(
                        sizeof(Data),
                        InputRate::INSTANCE
                );

                description.addAttribute(3, 0);
                description.addAttribute(3, 12);
                description.addAttribute(3, 24);
                description.addAttribute(1, 36);
                description.addAttribute(1, 40);
                description.addAttribute(1, 44);

                return description;
            }
        };

        PointLight();

        explicit PointLight(const std::shared_ptr<Model>& model);

        [[nodiscard]] const glm::vec3& getDiffuseColor() const;

        void setDiffuseColor(const glm::vec3& diffuseColor);

        [[nodiscard]] const glm::vec3& getSpecularColor() const;

        void setSpecularColor(const glm::vec3& specularColor);

        [[nodiscard]] float getConstantAttenuation() const;

        void setConstantAttenuation(float constantAttenuation);

        [[nodiscard]] float getLinearAttenuation() const;

        void setLinearAttenuation(float linearAttenuation);

        [[nodiscard]] float getQuadraticAttenuation() const;

        void setQuadraticAttenuation(float quadraticAttenuation);

        [[nodiscard]] float attenuationAt(float distance) const;

        void onStart() override;

        void onLateUpdate(float deltaTime) override;

        void drawEditor() override;

    };
    REGISTER_COMPONENT(PointLight, "Point Light")
}

#endif //NEON_POINTLIGHT_H
