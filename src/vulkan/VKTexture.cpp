//
// Created by gaelr on 10/11/2022.
//

#include "VKTexture.h"

#include <engine/Application.h>
#include <engine/structure/Room.h>

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
          _width(static_cast<int>(dummyInfo.image.width)),
          _height(static_cast<int>(dummyInfo.image.height)),
          _depth(static_cast<int>(dummyInfo.image.depth)),
          _mipmapLevels(dummyInfo.image.mipmaps),
          _layers(dummyInfo.image.layers),
          _stagingBuffer(std::make_unique<SimpleBuffer>(
              _vkApplication,
              VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
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
            createInfo.image.mipmaps =
                    std::floor(std::log2(std::max({_width, _height, _depth})));
            _mipmapLevels = createInfo.image.mipmaps;
        }

        VkFormat vkFormat = vc::vkFormat(createInfo.image.format);

        auto pair = vulkan_util::createImage(
            _vkApplication->getDevice(),
            _vkApplication->getPhysicalDevice(),
            createInfo.image,
            createInfo.imageView.viewType);

        _image = pair.first;
        _imageMemory = pair.second;

        //
        {
            CommandPoolHolder holder;
            CommandBuffer* buffer;
            if (createInfo.commandBuffer != nullptr) {
                buffer = createInfo.commandBuffer;
            }
            else {
                holder = application->getCommandManager().fetchCommandPool();
                buffer = holder.getPool().beginCommandBuffer(true);
            }
            VkCommandBuffer rawBuffer = buffer->getImplementation().
                    getCommandBuffer();

            vulkan_util::transitionImageLayout(
                _vkApplication,
                _image,
                vkFormat,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                createInfo.image.mipmaps,
                createInfo.image.layers,
                rawBuffer
            );

            vulkan_util::copyBufferToImage(
                _vkApplication,
                _stagingBuffer->getRaw(),
                _image,
                _width, _height, _depth,
                createInfo.image.layers,
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

            if (createInfo.commandBuffer == nullptr) {
                buffer->end();
                buffer->submit();
            }
        }

        // generateMipmaps transforms image to
        // VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL

        _imageView = vulkan_util::createImageView(
            _vkApplication->getDevice(),
            _image,
            vkFormat,
            VK_IMAGE_ASPECT_COLOR_BIT,
            createInfo.imageView
        );

        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(
            _vkApplication->getPhysicalDevice(),
            &properties
        );


        VkSamplerCreateInfo samplerInfo = vc::vkSamplerCreateInfo(
            createInfo.sampler,
            static_cast<float>(createInfo.image.mipmaps),
            properties.limits.maxSamplerAnisotropy);

        if (vkCreateSampler(_vkApplication->getDevice(), &samplerInfo, nullptr,
                            &_sampler) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create texture sampler!");
        }
    }

    VKTexture::VKTexture(Application* application,
                         VkImage image,
                         VkDeviceMemory memory,
                         VkImageView imageView,
                         VkImageLayout layout,
                         uint32_t width, uint32_t height, uint32_t depth,
                         const SamplerCreateInfo& sampler) : _vkApplication(
            dynamic_cast<AbstractVKApplication*>(
                application->getImplementation())),
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
            _vkApplication->getPhysicalDevice(),
            &properties
        );

        VkSamplerCreateInfo samplerInfo = vc::vkSamplerCreateInfo(
            sampler,
            static_cast<float>(_mipmapLevels),
            properties.limits.maxSamplerAnisotropy);

        if (vkCreateSampler(_vkApplication->getDevice(), &samplerInfo, nullptr,
                            &_sampler) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create texture sampler!");
        }
    }

    VKTexture::~VKTexture() {
        vkDestroySampler(_vkApplication->getDevice(), _sampler, nullptr);
        if (!_external) {
            vkDestroyImageView(_vkApplication->getDevice(), _imageView,
                               nullptr);
            vkDestroyImage(_vkApplication->getDevice(), _image, nullptr);
            vkFreeMemory(_vkApplication->getDevice(), _imageMemory, nullptr);
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
            std::cerr << "Image is already internal!" << std::endl;
            return;
        }
        _external = false;
    }

    void VKTexture::changeExternalImageView(
        int32_t width, int32_t height,
        VkImage image, VkDeviceMemory memory, VkImageView imageView) {
        if (!_external) {
            std::cerr << "The image view of an internal texture cannot"
                    "be changed!" << std::endl;
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
    }

    void VKTexture::updateData(const void* data, int32_t width, int32_t height,
                               int32_t depth, TextureFormat format) {
        if (_external) {
            std::cerr << "Couldn't update data of a texture"
                    " with an external handler" << std::endl;
            return;
        }

        if (_stagingBuffer == nullptr) {
            std::cerr << "Couldn't update data of a texture"
                    " staging buffer is not found because"
                    "the texture was external" << std::endl;
            return;
        }

        auto map = _stagingBuffer->map<char>();
        if (map.has_value()) {
            memcpy(map.value()->raw(), data, vc::pixelSize(format)
                                             * width * height * depth);
            map.value()->dispose();
        }

        auto vkFormat = vc::vkFormat(format);

        //
        {
            CommandPoolHolder holder = _application->getCommandManager()
                    .fetchCommandPool();
            CommandBuffer* buffer = holder.getPool().beginCommandBuffer(true);
            VkCommandBuffer rawBuffer = buffer->getImplementation().
                    getCommandBuffer();

            vulkan_util::transitionImageLayout(
                _vkApplication,
                _image,
                vkFormat,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                _mipmapLevels,
                _layers,
                rawBuffer
            );

            vulkan_util::copyBufferToImage(
                _vkApplication,
                _stagingBuffer->getRaw(),
                _image,
                width,
                height,
                depth,
                _layers,
                rawBuffer
            );

            vulkan_util::transitionImageLayout(
                _vkApplication,
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
}
