//
// Created by gaelr on 21/01/2023.
//

#ifndef NEON_IMGUITEST_H
#define NEON_IMGUITEST_H

#include <engine/Engine.h>

class ImGuiTestComponent : public Component {

    std::shared_ptr<SimpleFrameBuffer> _frameBuffer;
    ImVec2 _windowSize;

public:

    void onStart() override;

    void onUpdate(float deltaTime) override;

};


#endif //NEON_IMGUITEST_H
