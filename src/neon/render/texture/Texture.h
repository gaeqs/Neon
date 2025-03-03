//
// Created by gaelr on 23/10/2022.
//

#ifndef RVTRACKING_TEXTURE_H
#define RVTRACKING_TEXTURE_H


#include <memory>

#include <neon/structure/Asset.h>
#include <neon/render/texture/TextureCreateInfo.h>

#include <cmrc/cmrc.hpp>

#ifdef USE_VULKAN

#include <vulkan/VKTexture.h>

#endif

namespace neon {
    class Application;

    class Texture : public Asset {
    public:
#ifdef USE_VULKAN
        using Implementation = vulkan::VKTexture;
#endif

    private:
        Implementation _implementation;
        TextureFormat _format;
        SamplesPerTexel _samples;

    public:
        Texture(const Texture& other) = delete;

        Texture(Application* application,
                std::string name,
                const void* data,
                const TextureCreateInfo& createInfo = TextureCreateInfo());

#ifdef USE_VULKAN

        Texture(Application* application,
                std::string name,
                VkImage image,
                VkDeviceMemory memory,
                VkImageView imageView,
                VkImageLayout layout,
                uint32_t width, uint32_t height, uint32_t depth,
                TextureFormat format,
                SamplesPerTexel samples,
                const SamplerCreateInfo& createInfo = SamplerCreateInfo());

#endif


        [[nodiscard]] const Implementation& getImplementation() const;

        Implementation& getImplementation();

        [[nodiscard]] uint32_t getWidth() const;

        [[nodiscard]] uint32_t getHeight() const;

        [[nodiscard]] uint32_t getDepth() const;

        [[nodiscard]] TextureFormat getFormat() const;

        [[nodiscard]] SamplesPerTexel getSamples() const;

        void updateData(const char* data,
                        int32_t width, int32_t height, int32_t depth,
                        TextureFormat format);

        void fetchData(void* data, rush::Vec3i offset, rush::Vec<3, uint32_t> size,
                       uint32_t layersOffset, uint32_t layers);

        // region Static methods

        /**
         * Creates a new texture from a resource.
         * @param resource the resource.
         * @param createInfo the texture creation info.
         * @return a pointer to the new texture.
         */
        static std::unique_ptr<Texture> createTextureFromFile(
            Application* application,
            std::string name,
            const cmrc::file& resource,
            const TextureCreateInfo& createInfo = TextureCreateInfo());

        /**
         * Creates a new texture from a set of resources.
         *
         * This method is useful if you want to create cube boxes
         * or layered textures.
         *
         * All images must have the same size!
         *
         * @param resource the resources.
         * Their raw data will be concatenated.
         * @param createInfo the texture creation info.
         * @return a pointer to the new texture.
         */
        static std::unique_ptr<Texture> createTextureFromFiles(
            Application* application,
            std::string name,
            const std::vector<cmrc::file>& resources,
            const TextureCreateInfo& createInfo = TextureCreateInfo());

        /**
         * Creates a new texture from a file.
         * @param path the path of the file.
         * @param createInfo the texture creation info.
         * @return a pointer to the new texture.
         */
        static std::unique_ptr<Texture> createTextureFromFile(
            Application* application,
            std::string name,
            const std::string& path,
            const TextureCreateInfo& createInfo = TextureCreateInfo());

        /**
         * Creates a new texture from a set of files.
         *
         * This method is useful if you want to create cube boxes
         * or layered textures.
         *
         * All images must have the same size!
         *
         * @param paths the paths of the files.
         * Their raw data will be concatenated.
         * @param createInfo the texture creation info.
         * @return a pointer to the new texture.
         */
        static std::unique_ptr<Texture> createTextureFromFiles(
            Application* application,
            std::string name,
            const std::vector<std::string>& paths,
            const TextureCreateInfo& createInfo = TextureCreateInfo());

        /**
         * Creates a new texture from raw data.
         * @param data the data.
         * @param size the length of the data in bytes.
         * @param createInfo the texture creation info.
         * @return a pointer to the new texture.
         */
        static std::unique_ptr<Texture> createTextureFromFile(
            Application* application,
            std::string name,
            const void* data, uint32_t size,
            const TextureCreateInfo& createInfo = TextureCreateInfo());

        /**
         * Creates a new texture from a set of raw data pointers.
         *
         * This method is useful if you want to create cube boxes
         * or layered textures.
         *
         * All images must have the same size!
         *
         * @param data the data array.
         * Their raw data will be concatenated.
         * @param sizes the lengths of the data in bytes.
         * @param createInfo the texture creation info.
         * @return a pointer to the new texture.
         */
        static std::unique_ptr<Texture> createTextureFromFile(
            Application* application,
            std::string name,
            const std::vector<const void*>& data,
            const std::vector<uint32_t>& sizes,
            const TextureCreateInfo& createInfo = TextureCreateInfo());

        // endregion
    };
}

#endif //RVTRACKING_TEXTURE_H
