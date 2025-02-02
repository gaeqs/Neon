//
// Created by gaelr on 23/10/2022.
//

#include "Texture.h"

#include <utility>

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

#include <neon/structure/Room.h>

namespace neon {
    Texture::Texture(Application* application,
                     std::string name,
                     const void* data,
                     const TextureCreateInfo& createInfo) :
        Asset(typeid(Texture), std::move(name)),
        _implementation(application, data, createInfo),
        _format(createInfo.image.format),
        _samples(createInfo.image.samples) {}

#ifdef USE_VULKAN

    Texture::Texture(Application* application,
                     std::string name,
                     VkImage image,
                     VkDeviceMemory memory,
                     VkImageView imageView,
                     VkImageLayout layout,
                     uint32_t width, uint32_t height, uint32_t depth,
                     TextureFormat format,
                     SamplesPerTexel samples,
                     const SamplerCreateInfo& createInfo) :
        Asset(typeid(Texture), std::move(name)),
        _implementation(application, format, image, memory, imageView, layout,
                        width, height, depth, createInfo),
        _format(format),
        _samples(samples) {}

#endif

    const Texture::Implementation& Texture::getImplementation() const {
        return _implementation;
    }

    Texture::Implementation& Texture::getImplementation() {
        return _implementation;
    }

    uint32_t Texture::getWidth() const {
        return _implementation.getWidth();
    }

    uint32_t Texture::getHeight() const {
        return _implementation.getHeight();
    }

    uint32_t Texture::getDepth() const {
        return _implementation.getDepth();
    }

    TextureFormat Texture::getFormat() const {
        return _format;
    }

    SamplesPerTexel Texture::getSamples() const {
        return _samples;
    }

    void Texture::updateData(const char* data,
                             int32_t width, int32_t height, int32_t depth,
                             TextureFormat format) {
        _implementation.updateData(data, width, height, depth, format);
    }

    std::unique_ptr<Texture> Texture::createTextureFromFile(
        Application* application,
        std::string name,
        const cmrc::file& resource,
        const TextureCreateInfo& createInfo) {
        return createTextureFromFile(application,
                                     std::move(name),
                                     resource.begin(),
                                     resource.size(),
                                     createInfo);
    }

    std::unique_ptr<Texture> Texture::createTextureFromFiles(
        Application* application,
        std::string name,
        const std::vector<cmrc::file>& resources,
        const TextureCreateInfo& createInfo) {
        std::vector<const void*> data;
        std::vector<uint32_t> sizes;
        data.reserve(resources.size());
        sizes.reserve(resources.size());

        for (const auto& item: resources) {
            data.push_back(item.begin());
            sizes.push_back(item.size());
        }

        return createTextureFromFile(application, std::move(name),
                                     data, sizes, createInfo);
    }

    std::unique_ptr<Texture> Texture::createTextureFromFile(
        Application* application,
        std::string name,
        const std::string& path,
        const TextureCreateInfo& createInfo) {
        int32_t width, height, channels;
        auto ptr = stbi_load(path.c_str(), &width, &height, &channels, 4);

        TextureCreateInfo customInfo = createInfo;
        if (customInfo.image.width == 0) customInfo.image.width = width;
        if (customInfo.image.height == 0) customInfo.image.height = height;
        if (customInfo.image.depth == 0) customInfo.image.depth = 1;

        auto image = std::make_unique<Texture>(
            application,
            std::move(name),
            (const char*)ptr,
            customInfo
        );

        stbi_image_free(ptr);
        return image;
    }

    std::unique_ptr<Texture> Texture::createTextureFromFiles(
        Application* application,
        std::string name,
        const std::vector<std::string>& paths,
        const TextureCreateInfo& createInfo) {
        std::vector<int32_t> widths, heights;
        std::vector<stbi_uc*> pointers;
        widths.resize(paths.size());
        heights.resize(paths.size());
        pointers.resize(paths.size());

        uint32_t totalSize = 0;

        for (uint32_t i = 0; i < paths.size(); ++i) {
            int32_t channels;
            pointers[i] = stbi_load(paths[i].c_str(),
                                    &widths[i], &heights[i],
                                    &channels, 4);
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
        for (const auto& pointer: pointers) {
            stbi_image_free(pointer);
        }

        TextureCreateInfo customInfo = createInfo;
        if (customInfo.image.width == 0) customInfo.image.width = widths[0];
        if (customInfo.image.height == 0) customInfo.image.height = heights[0];
        if (customInfo.image.depth == 0) customInfo.image.depth = 1;

        auto image = std::make_unique<Texture>(
            application,
            std::move(name),
            (const char*)data,
            customInfo
        );

        delete[] data;
        return image;
    }


    std::unique_ptr<Texture> Texture::createTextureFromFile(
        Application* application,
        std::string name,
        const void* data, uint32_t size,
        const TextureCreateInfo& createInfo) {
        int32_t width, height, channels;
        auto ptr = stbi_load_from_memory(
            static_cast<const uint8_t*>(data),
            static_cast<int32_t>(size),
            &width, &height, &channels, 4);

        TextureCreateInfo customInfo = createInfo;
        if (customInfo.image.width == 0) customInfo.image.width = width;
        if (customInfo.image.height == 0) customInfo.image.height = height;
        if (customInfo.image.depth == 0) customInfo.image.depth = 1;

        auto image = std::make_unique<Texture>(
            application,
            std::move(name),
            (const char*)ptr,
            customInfo
        );

        stbi_image_free(ptr);
        return image;
    }

    std::unique_ptr<Texture> Texture::createTextureFromFile(
        Application* application,
        std::string name,
        const std::vector<const void*>& data,
        const std::vector<uint32_t>& sizes,
        const TextureCreateInfo& createInfo) {
        std::vector<int32_t> widths, heights;
        std::vector<stbi_uc*> pointers;
        widths.resize(data.size());
        heights.resize(data.size());
        pointers.resize(data.size());

        uint32_t totalSize = 0;

        for (uint32_t i = 0; i < data.size(); ++i) {
            int32_t channels;
            pointers[i] = stbi_load_from_memory(
                static_cast<const uint8_t*>(data[i]),
                static_cast<int32_t>(sizes[i]),
                &widths[i], &heights[i],
                &channels, 4);
            totalSize += widths[i] * heights[i] * 4;
            if (pointers[i] == nullptr) {
                throw std::runtime_error(
                    "Image " + std::to_string(i) + " is null!");
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
        for (const auto& pointer: pointers) {
            stbi_image_free(pointer);
        }

        TextureCreateInfo customInfo = createInfo;
        if (customInfo.image.width == 0) customInfo.image.width = widths[0];
        if (customInfo.image.height == 0) customInfo.image.height = heights[0];
        if (customInfo.image.depth == 0) customInfo.image.depth = 1;

        auto image = std::make_unique<Texture>(
            application,
            std::move(name),
            (const char*)flatData,
            customInfo
        );
        delete[] flatData;
        return image;
    }

    void Texture::fetchData(void* data, rush::Vec3i offset, rush::Vec<3, uint32_t> size,
                            uint32_t layersOffset,
                            uint32_t layers) {
        _implementation.fetchData(data, offset, size, layersOffset, layers);
    }
}
