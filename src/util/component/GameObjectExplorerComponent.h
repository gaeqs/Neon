//
// Created by gaelr on 27/01/2023.
//

#ifndef NEON_GAMEOBJECTEXPLORERCOMPONENT_H
#define NEON_GAMEOBJECTEXPLORERCOMPONENT_H

#include <engine/structure/Component.h>
#include <engine/structure/GameObject.h>
#include <engine/structure/IdentifiableWrapper.h>

namespace neon {
    class GameObjectExplorerComponent : public Component {

        IdentifiableWrapper<GameObject> _target;

        void drawGeneralSection() const;

        void drawTransformSection() const;

        void drawComponentsSection() const;

    public:

        [[nodiscard]] const IdentifiableWrapper<GameObject>& getTarget() const;

        void setTarget(const IdentifiableWrapper<GameObject>& target);

        void onPreDraw() override;

    };
    REGISTER_COMPONENT(GameObjectExplorerComponent, "GameObject Explorer")
}

#endif //NEON_GAMEOBJECTEXPLORERCOMPONENT_H
