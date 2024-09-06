//
// Created by gaelr on 27/01/2023.
//

#ifndef NEON_SCENETREECOMPONENT_H
#define NEON_SCENETREECOMPONENT_H

#include <neon/structure/Component.h>

#include <neon/util/component/GameObjectExplorerComponent.h>

namespace neon {
    class SceneTreeComponent : public Component {

        static constexpr float TIME_BEFORE_POPUPLATION = 3.0f;

        IdentifiableWrapper<GameObjectExplorerComponent> _explorer;
        std::vector<IdentifiableWrapper<GameObject>> _roots;
        float _timeBeforePopulation;

        bool recursiveTreePopulation(
                const std::unordered_set<IdentifiableWrapper<GameObject>>& objects);

        void popuplate();

    public:

        SceneTreeComponent(
                IdentifiableWrapper<GameObjectExplorerComponent> explorer);

        void onPreDraw() override;

    };
    REGISTER_COMPONENT(SceneTreeComponent, "Scene Tree")
}

#endif //NEON_SCENETREECOMPONENT_H
