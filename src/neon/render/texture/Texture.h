//
// Created by gaeqs on 27/05/2025.
//

#ifndef NEON_TEXTURE_H
#define NEON_TEXTURE_H

#include <string>
#include <optional>

#include <rush/rush.h>

#include <neon/structure/Application.h>
#include <neon/structure/Asset.h>
#include <neon/render/texture/TextureCreateInfo.h>
#include <neon/filesystem/CMRCFileSystem.h>
#include <neon/util/Result.h>

namespace neon
{
    class TextureCapabilityModifiable
    {
      public:
        virtual ~TextureCapabilityModifiable() = default;

        virtual Result<void, std::string> updateData(const std::byte* data, rush::Vec3ui offset, rush::Vec3ui size,
                                                     uint32_t layerOffset, uint32_t layers,
                                                     CommandBuffer* commandBuffer) = 0;

        virtual void resize(rush::Vec3ui dimensions) = 0;
    };

    class TextureCapabilityRead
    {
      public:
        virtual ~TextureCapabilityRead() = default;

        virtual void readData(std::byte* data, rush::Vec3ui offset, rush::Vec3ui size, uint32_t layerOffset,
                              uint32_t layers, TextureFormat format) const = 0;
    };

    class Texture : public Asset
    {
      public:
        explicit Texture(std::string name);

        ~Texture() override = default;

        [[nodiscard]] virtual rush::Vec3ui getDimensions() const = 0;

        [[nodiscard]] virtual TextureFormat getFormat() const = 0;

        [[nodiscard]] virtual SamplesPerTexel getSamplesPerTexel() const = 0;

        [[nodiscard]] virtual size_t getNumberOfMipmaps() const = 0;

        [[nodiscard]] virtual std::optional<const TextureCapabilityRead*> asReadable() const = 0;

        [[nodiscard]] virtual std::optional<TextureCapabilityModifiable*> asModifiable() = 0;

        [[nodiscard]] virtual void* getNativeHandle() = 0;

        [[nodiscard]] virtual const void* getNativeHandle() const = 0;

        /**
         * Creates a new texture from a resource.
         * @param resource the resource.
         * @param createInfo the texture creation info.
         * @return a pointer to the new texture.
         */
        static std::unique_ptr<Texture> createTextureFromFile(Application* application, std::string name,
                                                              const cmrc::file& resource,
                                                              const ImageCreateInfo& createInfo = ImageCreateInfo());

        /**
         * Creates a new texture from a resource.
         * @param resource the resource.
         * @param createInfo the texture creation info.
         * @return a pointer to the new texture.
         */
        static std::unique_ptr<Texture> createTextureFromFile(Application* application, std::string name,
                                                              const File& resource,
                                                              const ImageCreateInfo& createInfo = ImageCreateInfo());

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
        static std::unique_ptr<Texture> createTextureFromFiles(Application* application, std::string name,
                                                               const std::vector<cmrc::file>& resources,
                                                               const ImageCreateInfo& createInfo = ImageCreateInfo());

        /**
         * Creates a new texture from a file.
         * @param path the path of the file.
         * @param createInfo the texture creation info.
         * @return a pointer to the new texture.
         */
        static std::unique_ptr<Texture> createTextureFromFile(Application* application, std::string name,
                                                              const std::string& path,
                                                              ImageCreateInfo createInfo = ImageCreateInfo());

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
        static std::unique_ptr<Texture> createTextureFromFiles(Application* application, std::string name,
                                                               const std::vector<std::string>& paths,
                                                               ImageCreateInfo createInfo = ImageCreateInfo());

        /**
         * Creates a new texture from raw data.
         * @param data the data.
         * @param size the length of the data in bytes.
         * @param createInfo the texture creation info.
         * @return a pointer to the new texture.
         */
        static std::unique_ptr<Texture> createTextureFromFile(Application* application, std::string name,
                                                              const void* data, uint32_t size,
                                                              ImageCreateInfo createInfo = ImageCreateInfo());

        /**
         * Creates a new texture from a set of raw data pointers.
         *
         * This method is useful if you want to create cube boxes
         * or layered textures.*
         * All images must have the same size!
         *
         * @param data the data array.

         * Their raw data will be concatenated.
         * @param sizes the lengths of the data in bytes.
         * @param createInfo the texture creation info.
         * @return a pointer to the new texture.
         */
        static std::unique_ptr<Texture> createTextureFromFiles(Application* application, std::string name,
                                                               const std::vector<const void*>& data,
                                                               const std::vector<uint32_t>& sizes,
                                                               ImageCreateInfo createInfo = ImageCreateInfo());
    };
} // namespace neon

#endif // NEON_TEXTURE_H
