//
// Created by gaelr on 22/12/2022.
//

#ifndef NEON_VKRENDER_H
#define NEON_VKRENDER_H

#include <engine/render/RenderPassStrategy.h>

#include <util/ClusteredLinkedCollection.h>

class Application;

class VKApplication;

class Room;

class VKRender {

    VKApplication* _vkApplication;

public:

    VKRender(const VKRender& other) = delete;

    explicit VKRender(Application* application);

    void render(
            Room* room,
            const std::vector<RenderPassStrategy>& strategies)
            const;

};


#endif //NEON_VKRENDER_H
