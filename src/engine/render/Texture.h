//
// Created by gaelr on 23/10/2022.
//

#ifndef RVTRACKING_TEXTURE_H
#define RVTRACKING_TEXTURE_H


#include <engine/structure/Identifiable.h>
#include <engine/render/TextureCreateInfo.h>

#ifdef USE_VULKAN

#include <vulkan/VKTexture.h>

#endif

namespace neon {
    class Room;

    class Texture : public Identifiable {

        template<class T> friend
        class IdentifiableWrapper;

    public:
#ifdef USE_VULKAN
        using Implementation = vulkan::VKTexture;
#endif

    private:
        uint64_t _id;
        Implementation _implementation;

    public:

        Texture(const Texture& other) = delete;

        Texture(Room* room, const void* data,
                const TextureCreateInfo& createInfo = TextureCreateInfo());

#ifdef USE_VULKAN

        Texture(Room* room, VkImageView imageView, VkImageLayout layout,
                uint32_t width, uint32_t height, uint32_t depth,
                const SamplerCreateInfo& createInfo = SamplerCreateInfo());

#endif


        uint64_t getId() const override;

        const Implementation& getImplementation() const;

        Implementation& getImplementation();

        uint32_t getWidth() const;

        uint32_t getHeight() const;

        uint32_t getDepth() const;

        void updateData(const char* data,
                        int32_t width, int32_t height, int32_t depth,
                        TextureFormat format);

    };
}

#endif //RVTRACKING_TEXTURE_H
