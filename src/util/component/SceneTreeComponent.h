//
// Created by gaelr on 27/01/2023.
//

#ifndef NEON_SCENETREECOMPONENT_H
#define NEON_SCENETREECOMPONENT_H

#include <engine/structure/Component.h>

#include <util/component/GameObjectExplorerComponent.h>

class SceneTreeComponent : public Component {

    IdentifiableWrapper<GameObjectExplorerComponent> _explorer;

    bool recursiveTreePopulation(
            const std::unordered_set<IdentifiableWrapper<GameObject>>& objects);

public:

    SceneTreeComponent(
            IdentifiableWrapper<GameObjectExplorerComponent> explorer);

    void onPreDraw() override;

};


#endif //NEON_SCENETREECOMPONENT_H
