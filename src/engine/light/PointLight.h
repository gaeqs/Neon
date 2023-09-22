//
// Created by gaelr on 24/01/2023.
//

#ifndef NEON_POINTLIGHT_H
#define NEON_POINTLIGHT_H

#include <rush/rush.h>

#include <engine/structure/Component.h>
#include <engine/render/GraphicComponent.h>
#include <engine/model/Model.h>

namespace neon {

    class PointLight : public Component {

        IdentifiableWrapper<GraphicComponent> _graphicComponent;
        std::shared_ptr<Model> _customModel;

        rush::Vec3f _diffuseColor;
        rush::Vec3f _specularColor;
        float _constantAttenuation;
        float _linearAttenuation;
        float _quadraticAttenuation;
        float _radiance;

    public:

        struct Data {
            rush::Vec3f diffuseColor;
            rush::Vec3f specularColor;
            rush::Vec3f position;
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

        [[nodiscard]] const rush::Vec3f& getDiffuseColor() const;

        void setDiffuseColor(const rush::Vec3f& diffuseColor);

        [[nodiscard]] const rush::Vec3f& getSpecularColor() const;

        void setSpecularColor(const rush::Vec3f& specularColor);

        [[nodiscard]] float getRadiance() const;

        void setRadiance(float radiance);

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
