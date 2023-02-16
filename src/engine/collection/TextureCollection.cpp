//
// Created by grial on 21/10/22.
//

#include "TextureCollection.h"

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

namespace neon {

    TextureCollection::TextureCollection(Room* room)
            : IdentifiableCollection(room) {
    }

    IdentifiableWrapper<Texture>
    TextureCollection::createTextureFromFile(
            const cmrc::file& resource, const TextureCreateInfo& createInfo) {
        return createTextureFromFile(resource.begin(), resource.size(),
                                     createInfo);
    }

    IdentifiableWrapper<Texture> TextureCollection::createTextureFromFiles(
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

        return createTextureFromFile(data, sizes, createInfo);
    }

    IdentifiableWrapper<Texture>
    TextureCollection::createTextureFromFile(
            const std::string& path, const TextureCreateInfo& createInfo) {
        int32_t width, height, channels;
        auto ptr = stbi_load(path.c_str(), &width, &height, &channels, 4);

        TextureCreateInfo customInfo = createInfo;
        if (customInfo.image.width == 0) customInfo.image.width = width;
        if (customInfo.image.height == 0) customInfo.image.height = height;
        if (customInfo.image.depth == 0) customInfo.image.depth = 1;

        auto image = create((const char*) ptr, customInfo);
        stbi_image_free(ptr);
        return image;
    }

    IdentifiableWrapper<Texture>
    TextureCollection::createTextureFromFiles(
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

        auto image = create((const char*) data, customInfo);

        delete[] data;
        return image;
    }

    IdentifiableWrapper<Texture> TextureCollection::createTextureFromFile(
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

        auto image = create((const char*) ptr, customInfo);
        stbi_image_free(ptr);
        return image;
    }


    IdentifiableWrapper<Texture> TextureCollection::createTextureFromFile(
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

        auto image = create((const char*) flatData, customInfo);

        delete[] flatData;
        return image;
    }
}