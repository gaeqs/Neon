//
// Created by gaeqs on 27/05/2025.
//

#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#ifdef USE_VULKAN
    #include <vulkan/render/texture/VKSimpleTexture.h>
#endif
namespace neon
{
    Texture::Texture(std::string name) :
        Asset(typeid(Texture), std::move(name))
    {
    }

    std::shared_ptr<Texture> Texture::createFromRawData(Application* application, std::string name, const void* data,
                                                        const ImageCreateInfo& createInfo, CommandBuffer* commandBuffer)
    {
#ifdef USE_VULKAN
        auto image = std::make_unique<vulkan::VKSimpleTexture>(application, std::move(name), createInfo,
                                                               static_cast<const std::byte*>(data), commandBuffer);
#else
        std::unique_ptr<Texture> image = nullptr;
#endif
        return image;
    }

    std::shared_ptr<Texture> Texture::createTextureFromFile(Application* application, std::string name,
                                                            const cmrc::file& resource,
                                                            const ImageCreateInfo& createInfo,
                                                            CommandBuffer* commandBuffer)
    {
        return createTextureFromFile(application, std::move(name), resource.begin(),
                                     static_cast<uint32_t>(resource.size()), createInfo, commandBuffer);
    }

    std::shared_ptr<Texture> Texture::createTextureFromFile(Application* application, std::string name,
                                                            const File& resource, const ImageCreateInfo& createInfo,
                                                            CommandBuffer* commandBuffer)
    {
        return createTextureFromFile(application, std::move(name), resource.getData(),
                                     static_cast<uint32_t>(resource.getSize()), createInfo, commandBuffer);
    }

    std::shared_ptr<Texture> Texture::createTextureFromFiles(Application* application, std::string name,
                                                             const std::vector<cmrc::file>& resources,
                                                             const ImageCreateInfo& createInfo,
                                                             CommandBuffer* commandBuffer)
    {
        std::vector<const void*> data;
        std::vector<uint32_t> sizes;
        data.reserve(resources.size());
        sizes.reserve(resources.size());

        for (const auto& item : resources) {
            data.push_back(item.begin());
            sizes.push_back(static_cast<uint32_t>(item.size()));
        }

        return createTextureFromFiles(application, std::move(name), data, sizes, createInfo, commandBuffer);
    }

    std::shared_ptr<Texture> Texture::createTextureFromFile(Application* application, std::string name,
                                                            const std::string& path, ImageCreateInfo createInfo,
                                                            CommandBuffer* commandBuffer)
    {
        int32_t width, height, channels;
        auto ptr = stbi_load(path.c_str(), &width, &height, &channels, 4);

        if (createInfo.width == 0) {
            createInfo.width = width;
        }
        if (createInfo.height == 0) {
            createInfo.height = height;
        }
        if (createInfo.depth == 0) {
            createInfo.depth = 1;
        }

#ifdef USE_VULKAN
        auto image = std::make_unique<vulkan::VKSimpleTexture>(application, std::move(name), createInfo,
                                                               reinterpret_cast<std::byte*>(ptr), commandBuffer);
#else
        std::unique_ptr<Texture> image = nullptr;
#endif
        stbi_image_free(ptr);
        return image;
    }

    std::shared_ptr<Texture> Texture::createTextureFromFiles(Application* application, std::string name,
                                                             const std::vector<std::string>& paths,
                                                             ImageCreateInfo createInfo, CommandBuffer* commandBuffer)
    {
        std::vector<int32_t> widths, heights;
        std::vector<stbi_uc*> pointers;
        widths.resize(paths.size());
        heights.resize(paths.size());
        pointers.resize(paths.size());

        uint32_t totalSize = 0;

        for (uint32_t i = 0; i < paths.size(); ++i) {
            int32_t channels;
            pointers[i] = stbi_load(paths[i].c_str(), &widths[i], &heights[i], &channels, 4);
            totalSize += widths[i] * heights[i] * 4;
            if (pointers[i] == nullptr) {
                throw std::runtime_error("Image " + paths[i] + " is null!");
            }
        }

        // Flat data
        auto* data = new stbi_uc[totalSize];
        size_t offset = 0;
        for (int i = 0; i < pointers.size(); ++i) {
            size_t size = widths[i] * heights[i] * 4;
            std::memcpy(data + offset, pointers[i], size);
            offset += size;
        }

        // Free pointers
        for (const auto& pointer : pointers) {
            stbi_image_free(pointer);
        }

        if (createInfo.width == 0) {
            createInfo.width = widths[0];
        }
        if (createInfo.height == 0) {
            createInfo.height = heights[0];
        }
        if (createInfo.depth == 0) {
            createInfo.depth = 1;
        }
#ifdef USE_VULKAN
        auto image = std::make_unique<vulkan::VKSimpleTexture>(application, std::move(name), createInfo,
                                                               reinterpret_cast<std::byte*>(data), commandBuffer);
#else
        std::unique_ptr<Texture> image = nullptr;
#endif

        delete[] data;
        return image;
    }

    std::shared_ptr<Texture> Texture::createTextureFromFile(Application* application, std::string name,
                                                            const void* data, uint32_t size, ImageCreateInfo createInfo,
                                                            CommandBuffer* commandBuffer)
    {
        int32_t width, height, channels;
        auto ptr = stbi_load_from_memory(static_cast<const uint8_t*>(data), static_cast<int32_t>(size), &width, &height,
                                         &channels, 4);

        if (createInfo.width == 0) {
            createInfo.width = width;
        }
        if (createInfo.height == 0) {
            createInfo.height = height;
        }
        if (createInfo.depth == 0) {
            createInfo.depth = 1;
        }

#ifdef USE_VULKAN
        auto image = std::make_unique<vulkan::VKSimpleTexture>(application, std::move(name), createInfo,
                                                               reinterpret_cast<std::byte*>(ptr), commandBuffer);
#else
        std::unique_ptr<Texture> image = nullptr;
#endif
        stbi_image_free(ptr);
        return image;
    }

    std::shared_ptr<Texture> Texture::createTextureFromFiles(Application* application, std::string name,
                                                             const std::vector<const void*>& data,
                                                             const std::vector<uint32_t>& sizes,
                                                             ImageCreateInfo createInfo, CommandBuffer* commandBuffer)
    {
        std::vector<int32_t> widths, heights;
        std::vector<stbi_uc*> pointers;
        widths.resize(data.size());
        heights.resize(data.size());
        pointers.resize(data.size());

        uint32_t totalSize = 0;

        for (uint32_t i = 0; i < data.size(); ++i) {
            int32_t channels;
            pointers[i] = stbi_load_from_memory(static_cast<const uint8_t*>(data[i]), static_cast<int32_t>(sizes[i]),
                                                &widths[i], &heights[i], &channels, 4);
            totalSize += widths[i] * heights[i] * 4;
            if (pointers[i] == nullptr) {
                throw std::runtime_error("Image " + std::to_string(i) + " is null!");
            }
        }

        // Flat flatData
        auto* flatData = new stbi_uc[totalSize];
        size_t offset = 0;
        for (int i = 0; i < pointers.size(); ++i) {
            size_t size = widths[i] * heights[i] * 4;
            std::memcpy(flatData + offset, pointers[i], size);
            offset += size;
        }

        // Free pointers
        for (const auto& pointer : pointers) {
            stbi_image_free(pointer);
        }

        if (createInfo.width == 0) {
            createInfo.width = widths[0];
        }
        if (createInfo.height == 0) {
            createInfo.height = heights[0];
        }
        if (createInfo.depth == 0) {
            createInfo.depth = 1;
        }
#ifdef USE_VULKAN
        auto image = std::make_unique<vulkan::VKSimpleTexture>(application, std::move(name), createInfo,
                                                               reinterpret_cast<std::byte*>(flatData), commandBuffer);
#else
        std::unique_ptr<Texture> image = nullptr;
#endif

        delete[] flatData;
        return image;
    }
} // namespace neon