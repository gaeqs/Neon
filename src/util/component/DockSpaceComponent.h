//
// Created by gaelr on 21/01/2023.
//

#ifndef NEON_DOCKSPACECOMPONENT_H
#define NEON_DOCKSPACECOMPONENT_H

#include <engine/structure/Component.h>

class DockSpaceComponent : public Component {

public:

    void onUpdate(float deltaTime) override;

};
REGISTER_COMPONENT(DockSpaceComponent, "Dock Space")


#endif //NEON_DOCKSPACECOMPONENT_H
