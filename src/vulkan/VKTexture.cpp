//
// Created by gaelr on 10/11/2022.
//

#include "VKTexture.h"

#include <neon/structure/Application.h>
#include <neon/structure/Room.h>

#include <vulkan/util/VKUtil.h>
#include <vulkan/util/VulkanConversions.h>

namespace vc = neon::vulkan::conversions;

namespace neon::vulkan {
    VKTexture::VKTexture(Application* application,
                         const void* data,
                         const TextureCreateInfo& dummyInfo)
        : _application(application),
          _vkApplication(dynamic_cast<AbstractVKApplication*>(
              application->getImplementation())),
          _format(dummyInfo.image.format),
          _width(static_cast<int>(dummyInfo.image.width)),
          _height(static_cast<int>(dummyInfo.image.height)),
          _depth(static_cast<int>(dummyInfo.image.depth)),
          _mipmapLevels(dummyInfo.image.mipmaps),
          _layers(dummyInfo.image.layers),
          _stagingBuffer(std::make_unique<SimpleBuffer>(
              _vkApplication,
              VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
              data,
              vc::pixelSize(dummyInfo.image.format)
              * _width * _height * _depth * _layers
          )),
          _image(VK_NULL_HANDLE),
          _imageMemory(VK_NULL_HANDLE),
          _imageView(VK_NULL_HANDLE),
          _sampler(VK_NULL_HANDLE),
          _layout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL),
          _external(false),
          _externalDirtyFlag(1) {
        // Create copy: useful if we want to modify data.
        TextureCreateInfo createInfo = dummyInfo;

        if (createInfo.image.mipmaps == 0) {
            // Mipmap level not defined.

            uint32_t maxDim = std::max({_width, _height, _depth});
            createInfo.image.mipmaps = std::bit_width(maxDim) - 1;
            _mipmapLevels = createInfo.image.mipmaps;
        }

        VkFormat vkFormat = vc::vkFormat(createInfo.image.format);

        auto pair = vulkan_util::createImage(
            _vkApplication->getDevice()->getRaw(),
            _vkApplication->getPhysicalDevice().getRaw(),
            createInfo.image,
            createInfo.imageView.viewType);

        _image = pair.first;
        _imageMemory = pair.second;

        //
        {
            CommandPoolHolder holder;
            const CommandBuffer* buffer;
            CommandBuffer* internalBuffer = nullptr;
            if (createInfo.commandBuffer != nullptr) {
                buffer = createInfo.commandBuffer;
            } else {
                holder = application->getCommandManager().fetchCommandPool();
                internalBuffer = holder.getPool().beginCommandBuffer(true);
                buffer = internalBuffer;
            }
            VkCommandBuffer rawBuffer = buffer->getImplementation().
                    getCommandBuffer();

            vulkan_util::transitionImageLayout(
                _image,
                vkFormat,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                createInfo.image.mipmaps,
                createInfo.image.layers,
                rawBuffer
            );

            vulkan_util::copyBufferToImage(
                _stagingBuffer->getRaw(),
                _image,
                _width,
                _height, _depth, createInfo.image.layers,
                rawBuffer
            );

            vulkan_util::generateMipmaps(
                _vkApplication,
                _image,
                _width, _height, _depth,
                createInfo.image.mipmaps,
                createInfo.image.layers,
                rawBuffer
            );

            if (internalBuffer != nullptr) {
                internalBuffer->end();
                internalBuffer->submit();
            }
        }

        // generateMipmaps transforms image to
        // VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL

        _imageView = vulkan_util::createImageView(
            _vkApplication->getDevice()->getRaw(),
            _image,
            vkFormat,
            VK_IMAGE_ASPECT_COLOR_BIT,
            createInfo.imageView
        );

        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(
            _vkApplication->getPhysicalDevice().getRaw(),
            &properties
        );


        VkSamplerCreateInfo samplerInfo = vc::vkSamplerCreateInfo(
            createInfo.sampler,
            static_cast<float>(createInfo.image.mipmaps),
            properties.limits.maxSamplerAnisotropy);

        if (vkCreateSampler(_vkApplication->getDevice()->getRaw(), &samplerInfo,
                            nullptr,
                            &_sampler) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create texture sampler!");
        }
    }

    VKTexture::VKTexture(Application* application,
                         TextureFormat format,
                         VkImage image,
                         VkDeviceMemory memory,
                         VkImageView imageView,
                         VkImageLayout layout,
                         uint32_t width, uint32_t height, uint32_t depth,
                         const SamplerCreateInfo& sampler)
        : _application(application),
          _vkApplication(dynamic_cast<AbstractVKApplication*>(application->getImplementation())),
          _format(format),
          _width(static_cast<int>(width)),
          _height(static_cast<int>(height)),
          _depth(static_cast<int>(depth)),
          _mipmapLevels(0),
          _layers(1),
          _stagingBuffer(nullptr),
          _image(image),
          _imageMemory(memory),
          _imageView(imageView),
          _sampler(VK_NULL_HANDLE),
          _layout(layout),
          _external(true),
          _externalDirtyFlag(1) {
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(
            _vkApplication->getPhysicalDevice().getRaw(),
            &properties
        );

        VkSamplerCreateInfo samplerInfo = vc::vkSamplerCreateInfo(
            sampler,
            static_cast<float>(_mipmapLevels),
            properties.limits.maxSamplerAnisotropy);

        if (vkCreateSampler(_vkApplication->getDevice()->getRaw(), &samplerInfo,
                            nullptr,
                            &_sampler) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create texture sampler!");
        }
    }

    VKTexture::~VKTexture() {
        vkDestroySampler(_vkApplication->getDevice()->getRaw(), _sampler,
                         nullptr);
        if (!_external) {
            vkDestroyImageView(_vkApplication->getDevice()->getRaw(),
                               _imageView,
                               nullptr);
            vkDestroyImage(_vkApplication->getDevice()->getRaw(), _image,
                           nullptr);
            vkFreeMemory(_vkApplication->getDevice()->getRaw(), _imageMemory,
                         nullptr);
        }
    }

    int32_t VKTexture::getWidth() const {
        return _width;
    }

    int32_t VKTexture::getHeight() const {
        return _height;
    }

    int32_t VKTexture::getDepth() const {
        return _depth;
    }

    VkImage VKTexture::getImage() const {
        return _image;
    }

    VkDeviceMemory VKTexture::getMemory() const {
        return _imageMemory;
    }

    VkImageView VKTexture::getImageView() const {
        return _imageView;
    }

    VkSampler VKTexture::getSampler() const {
        return _sampler;
    }

    VkImageLayout VKTexture::getLayout() const {
        return _layout;
    }

    uint32_t VKTexture::getExternalDirtyFlag() const {
        return _externalDirtyFlag;
    }

    void VKTexture::makeInternal() {
        if (!_external) {
            logger.warning(MessageBuilder().group("vulkan").print("Image is already internal!"));
            return;
        }
        _external = false;
    }

    void VKTexture::changeExternalImageView(
        int32_t width, int32_t height,
        VkImage image, VkDeviceMemory memory, VkImageView imageView) {
        if (!_external) {
            logger.error(MessageBuilder().group("vulkan").print("The image view of an internal"
                    " texture cannot be changed!"));
            return;
        }

        _width = width;
        _height = height;
        _image = image;
        _imageMemory = memory;
        _imageView = imageView;
        _externalDirtyFlag++;
        if (_externalDirtyFlag == 0) {
            _externalDirtyFlag++;
        }

        if (_stagingBuffer != nullptr) {
            // Staging buffer becomes invalid because the image size has changed.
            _stagingBuffer = nullptr;
        }
    }

    void VKTexture::updateData(const void* data, int32_t width, int32_t height,
                               int32_t depth, TextureFormat format) {
        if (_external) {
            logger.error(MessageBuilder()
                .group("vulkan")
                .print("Couldn't update data of a texture"
                    " with an external handler"));
            return;
        }

        if (_stagingBuffer == nullptr) {
            _stagingBuffer = std::make_unique<SimpleBuffer>(
                _vkApplication,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                nullptr,
                vc::pixelSize(_format) * _width * _height * _depth * _layers
            );
        }

        auto map = _stagingBuffer->map<char>();
        if (map.has_value()) {
            memcpy(map.value()->raw(), data, vc::pixelSize(format)
                                             * width * height * depth);
            map.value()->dispose();
        }

        _format = format;
        auto vkFormat = vc::vkFormat(format);

        //
        {
            CommandPoolHolder holder = _application->getCommandManager()
                    .fetchCommandPool();
            CommandBuffer* buffer = holder.getPool().beginCommandBuffer(true);
            VkCommandBuffer rawBuffer = buffer->getImplementation().
                    getCommandBuffer();

            vulkan_util::transitionImageLayout(
                _image,
                vkFormat,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                _mipmapLevels,
                _layers,
                rawBuffer
            );

            vulkan_util::copyBufferToImage(
                _stagingBuffer->getRaw(),
                _image,
                width,
                height,
                depth,
                _layers,
                rawBuffer
            );

            vulkan_util::transitionImageLayout(
                _image,
                vkFormat,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                _mipmapLevels,
                _layers,
                rawBuffer
            );
        }
    }

    void VKTexture::fetchData(void* data, rush::Vec3i offset, rush::Vec<3, uint32_t> size,
                              uint32_t layersOffset, uint32_t layers) {
        if (_stagingBuffer == nullptr) {
            _stagingBuffer = std::make_unique<SimpleBuffer>(
                _vkApplication,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                nullptr,
                vc::pixelSize(_format) * _width * _height * _depth * _layers
            );
        }

        CommandPoolHolder holder = _application->getCommandManager().fetchCommandPool();
        CommandBuffer* cmd = holder.getPool().beginCommandBuffer(true);

        auto vkFormat = vc::vkFormat(_format);

        vulkan_util::transitionImageLayout(
            _image,
            vkFormat,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            _mipmapLevels,
            _layers,
            cmd->getImplementation().getCommandBuffer()
        );

        vulkan_util::copyImageToBuffer(
            _stagingBuffer->getRaw(), _image,
            offset, size,
            layersOffset, layers,
            cmd->getImplementation().getCommandBuffer()
        );

        vulkan_util::transitionImageLayout(
            _image,
            vkFormat,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            _mipmapLevels,
            _layers,
            cmd->getImplementation().getCommandBuffer()
        );

        cmd->end();
        cmd->submit();
        cmd->wait();

        auto map = _stagingBuffer->map<uint8_t>();
        if (!map.has_value()) return;
        auto bufferData = map.value()->raw();
        memcpy(data, bufferData, size[0] * size[1] * size[2] * layers * conversions::pixelSize(_format));
    }
}
