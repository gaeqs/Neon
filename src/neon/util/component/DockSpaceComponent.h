//
// Created by gaelr on 21/01/2023.
//

#ifndef NEON_DOCKSPACECOMPONENT_H
#define NEON_DOCKSPACECOMPONENT_H

#include <neon/structure/Component.h>

namespace neon
{
    class DockSpaceComponent : public Component
    {
        int dockFlags;

      public:
        explicit DockSpaceComponent(bool topBar = false);

        void onUpdate(float deltaTime) override;
    };

    REGISTER_COMPONENT(DockSpaceComponent, "Dock Space")
} // namespace neon

#endif //NEON_DOCKSPACECOMPONENT_H
