//
// Created by gaelr on 24/01/2023.
//

#ifndef NEON_DIRECTIONALLIGHT_H
#define NEON_DIRECTIONALLIGHT_H

#include <rush/rush.h>

#include <neon/structure/Component.h>
#include <neon/render/GraphicComponent.h>
#include <neon/render/model/Model.h>

namespace neon {
    class DirectionalLight : public Component {

        IdentifiableWrapper<GraphicComponent> _graphicComponent;
        std::shared_ptr<Model> _customModel;

        rush::Vec3f _diffuseColor;
        rush::Vec3f _specularColor;
        float _radiance;

    public:

        struct Data {
            rush::Vec3f diffuseColor;
            rush::Vec3f specularColor;
            rush::Vec3f direction;

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

        explicit DirectionalLight(const std::shared_ptr<Model>& model);

        [[nodiscard]] const rush::Vec3f& getDiffuseColor() const;

        void setDiffuseColor(const rush::Vec3f& diffuseColor);

        [[nodiscard]] const rush::Vec3f& getSpecularColor() const;

        void setSpecularColor(const rush::Vec3f& specularColor);

        [[nodiscard]] float getRadiance() const;

        void setRadiance(float radiance);

        void onStart() override;

        void onLateUpdate(float deltaTime) override;

        void drawEditor() override;

    };
    REGISTER_COMPONENT(DirectionalLight, "Directional Light")
}

#endif //NEON_DIRECTIONALLIGHT_H
