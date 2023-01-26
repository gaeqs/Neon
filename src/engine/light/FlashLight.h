//
// Created by gaelr on 24/01/2023.
//

#ifndef NEON_FLASHLIGHT_H
#define NEON_FLASHLIGHT_H

#include <engine/structure/Component.h>
#include <engine/render/GraphicComponent.h>
#include <engine/model/Model.h>

class FlashLight : public Component {

    IdentifiableWrapper<GraphicComponent> _graphicComponent;
    IdentifiableWrapper<Model> _customModel;

    glm::vec3 _diffuseColor;
    glm::vec3 _specularColor;
    float _constantAttenuation;
    float _linearAttenuation;
    float _quadraticAttenuation;
    float _cutOff;
    float _outerCutOff;

public:

    struct Data {
        glm::vec3 diffuseColor;
        glm::vec3 specularColor;
        glm::vec3 position;
        glm::vec3 direction;
        float constantAttenuation;
        float linearAttenuation;
        float quadraticAttenuation;
        float cutOff;
        float outerCutOff;

        static InputDescription getDescription() {
            InputDescription description(
                    sizeof(Data),
                    InputRate::INSTANCE
            );

            description.addAttribute(3, 0);
            description.addAttribute(3, 12);
            description.addAttribute(3, 24);
            description.addAttribute(3, 36);
            description.addAttribute(1, 48);
            description.addAttribute(1, 52);
            description.addAttribute(1, 56);
            description.addAttribute(1, 60);
            description.addAttribute(1, 64);

            return description;
        }
    };

    FlashLight();

    explicit FlashLight(const IdentifiableWrapper<Model>& model);

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

    [[nodiscard]] float getCutOff() const;

    void setCutOff(float cutOff);

    [[nodiscard]] float getOuterCutOff() const;

    void setOuterCutOff(float outerCutOff);

    void onStart() override;

    void onLateUpdate(float deltaTime) override;

};


#endif //NEON_FLASHLIGHT_H
