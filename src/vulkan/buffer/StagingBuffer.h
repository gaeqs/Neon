//
// Created by grial on 17/10/22.
//

#ifndef VULKANTEST_STAGINGBUFFER_H
#define VULKANTEST_STAGINGBUFFER_H


#include <utility>
#include <vector>
#include "Buffer.h"
#include "SimpleBuffer.h"

#include <engine/render/CommandBuffer.h>

#include <vulkan/util/VKUtil.h>
#include <vulkan/buffer/SimpleBuffer.h>
#include <vulkan/AbstractVKApplication.h>

namespace neon::vulkan {
    class StagingBuffer;

    template<class T>
    class StagingBufferMap : public BufferMap<T> {
        VkCommandBuffer _commandBuffer;

        std::shared_ptr<SimpleBuffer> _stagingBuffer;
        SimpleBuffer& _deviceBuffer;

        std::shared_ptr<BufferMap<T>> _map;
        std::optional<Range<uint32_t>> _range;

    public:
        StagingBufferMap(const StagingBufferMap& other) = delete;

        StagingBufferMap(VkCommandBuffer commandBuffer,
                         std::shared_ptr<SimpleBuffer> stagingBuffer,
                         SimpleBuffer& deviceBuffer)
            : BufferMap<T>(),
              _commandBuffer(commandBuffer),
              _stagingBuffer(std::move(stagingBuffer)),
              _deviceBuffer(deviceBuffer),
              _map(_stagingBuffer->map<T>().value()) {
        }

        StagingBufferMap(VkCommandBuffer commandBuffer,
                         std::shared_ptr<SimpleBuffer> stagingBuffer,
                         SimpleBuffer& deviceBuffer,
                         Range<uint32_t> range)
            : BufferMap<T>(),
              _commandBuffer(commandBuffer),
              _stagingBuffer(std::move(stagingBuffer)),
              _deviceBuffer(deviceBuffer),
              _map(_stagingBuffer->map<T>(range).value()),
              _range(range) {
        }

        ~StagingBufferMap() override {
            dispose();
        };

        T& operator[](size_t index) override {
            return _map->raw()[index];
        }

        T operator[](size_t index) const override {
            return _map->raw()[index];
        }

        T* raw() override {
            return _map->raw();
        }

        void dispose() override {
            if (_map->raw() == nullptr) return;
            _map->dispose();
            if (_range.has_value()) {
                vulkan_util::copyBuffer(
                    _commandBuffer,
                    _stagingBuffer->getRaw(),
                    _deviceBuffer.getRaw(),
                    _range.value().getFrom(),
                    _range.value().getFrom(),
                    _range.value().size()
                );
            } else {
                vulkan_util::copyBuffer(
                    _commandBuffer,
                    _stagingBuffer->getRaw(),
                    _deviceBuffer.getRaw(),
                    _deviceBuffer.size()
                );
            }
        }
    };

    class StagingBuffer : public Buffer {
        AbstractVKApplication* _application;

        std::vector<std::shared_ptr<SimpleBuffer>> _stagingBuffers;
        SimpleBuffer _deviceBuffer;

        std::optional<std::shared_ptr<BufferMap<char>>> rawMap(
            const CommandBuffer* commandBuffer = nullptr) override;

        std::optional<std::shared_ptr<BufferMap<char>>> rawMap(
            Range<uint32_t> range,
            const CommandBuffer* commandBuffer = nullptr) override;

    public:
        StagingBuffer(const StagingBuffer& other) = delete;

        template<class T>
        StagingBuffer(AbstractVKApplication* application,
                      VkBufferUsageFlags usage,
                      const std::vector<T>& data) : _application(application),
                                                    _stagingBuffers(),
                                                    _deviceBuffer(_application,
                                                        VK_BUFFER_USAGE_TRANSFER_DST_BIT
                                                        | usage,
                                                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                        data) {
            for (uint32_t i = 0; i < _application->getMaxFramesInFlight(); ++
                 i) {
                _stagingBuffers.push_back(std::make_shared<SimpleBuffer>(
                    _application,
                    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                    data
                ));
            }

            map<T>();
            // Transfers all data from the staging buffer to the device buffer.
        }

        StagingBuffer(AbstractVKApplication* application,
                      VkBufferUsageFlags usage, uint32_t sizeInBytes);

        ~StagingBuffer() override = default;

        size_t size() const override;

        bool canBeWrittenOn() const override;

        VkBuffer getRaw() const override;

        AbstractVKApplication* getApplication() const override;

        SimpleBuffer& getDeviceBuffer();

        const SimpleBuffer& getDeviceBuffer() const;
    };
}


#endif //VULKANTEST_STAGINGBUFFER_H
