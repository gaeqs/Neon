//
// Created by gaelr on 23/10/2022.
//

#ifndef RVTRACKING_TEXTURE_H
#define RVTRACKING_TEXTURE_H


#include <engine/structure/Identifiable.h>
#include "TextureFormat.h"

#ifdef USE_VULKAN

#include "vulkan/VKTexture.h"

#endif


class Room;

class Texture : public Identifiable {

    template<class T> friend
    class IdentifiableWrapper;

public:
#ifdef USE_VULKAN
    using Implementation = VKTexture;
#endif

private:
    uint64_t _id;
    Implementation _implementation;

public:

    Texture(const Texture& other) = delete;

    Texture(Room* room, const void* data,
            int32_t width, int32_t height, TextureFormat format);

#ifdef USE_VULKAN

    Texture(Room* room, int32_t width, int32_t height,
            VkImageView imageView,
            VkImageLayout layout);

#endif


    uint64_t getId() const override;

    const Implementation& getImplementation() const;

    Implementation& getImplementation();

    uint32_t getWidth() const;

    uint32_t getHeight() const;

    void updateData(const char* data, int32_t width, int32_t height,
                    TextureFormat format);

};


#endif //RVTRACKING_TEXTURE_H
