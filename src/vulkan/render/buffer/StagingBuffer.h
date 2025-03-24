//
// Created by grial on 17/10/22.
//

#ifndef VULKANTEST_STAGINGBUFFER_H
#define VULKANTEST_STAGINGBUFFER_H

#include <utility>
#include <vector>
#include "Buffer.h"
#include "SimpleBuffer.h"

#include <neon/render/buffer/CommandBuffer.h>
#include <neon/logging/Logger.h>

#include <vulkan/util/VKUtil.h>
#include <vulkan/AbstractVKApplication.h>

namespace neon::vulkan
{
    class StagingBuffer;

    template<class T>
    class StagingBufferMap : public BufferMap<T>
    {
        CommandPoolHolder _poolHolder;

        const CommandBuffer* _externalCommandBuffer;
        CommandBuffer* _internalCommandBuffer;

        std::shared_ptr<SimpleBuffer> _stagingBuffer;
        SimpleBuffer& _deviceBuffer;

        std::shared_ptr<BufferMap<T>> _map;
        std::optional<Range<uint32_t>> _range;

        bool _disposed = false;

      public:
        StagingBufferMap(const StagingBufferMap& other) = delete;

        StagingBufferMap(StagingBufferMap&& other) noexcept :
            _poolHolder(std::move(other._poolHolder)),
            _externalCommandBuffer(other._externalCommandBuffer),
            _internalCommandBuffer(other._internalCommandBuffer),
            _stagingBuffer(std::move(other._stagingBuffer)),
            _deviceBuffer(other._deviceBuffer),
            _map(std::move(other._map)),
            _range(other._range),
            _disposed(other._disposed)
        {
            other._disposed = true;
        }

        StagingBufferMap(Application* application, const CommandBuffer* commandBuffer,
                         std::shared_ptr<SimpleBuffer> stagingBuffer, SimpleBuffer& deviceBuffer) :
            BufferMap<T>(),
            _externalCommandBuffer(commandBuffer),
            _internalCommandBuffer(nullptr),
            _stagingBuffer(std::move(stagingBuffer)),
            _deviceBuffer(deviceBuffer),
            _map(_stagingBuffer->map<T>().value())
        {
            if (_externalCommandBuffer == nullptr) {
                _poolHolder = application->getCommandManager().fetchCommandPool();
                _internalCommandBuffer = _poolHolder.getPool().beginCommandBuffer(true);
            }
        }

        StagingBufferMap(Application* application, const CommandBuffer* commandBuffer,
                         std::shared_ptr<SimpleBuffer> stagingBuffer, SimpleBuffer& deviceBuffer,
                         Range<uint32_t> range) :
            BufferMap<T>(),
            _externalCommandBuffer(commandBuffer),
            _internalCommandBuffer(nullptr),
            _stagingBuffer(std::move(stagingBuffer)),
            _deviceBuffer(deviceBuffer),
            _map(_stagingBuffer->map<T>(range).value()),
            _range(range)
        {
            if (_externalCommandBuffer == nullptr) {
                _poolHolder = application->getCommandManager().fetchCommandPool();
                _internalCommandBuffer = _poolHolder.getPool().beginCommandBuffer(true);
            }
        }

        ~StagingBufferMap() override
        {
            disposeStagingBuffer();
        };

        T& operator[](size_t index) override
        {
            return _map->raw()[index];
        }

        T operator[](size_t index) const override
        {
            return _map->raw()[index];
        }

        T* raw() override
        {
            return _map->raw();
        }

        void disposeStagingBuffer()
        {
            if (_disposed) {
                logger.warning(MessageBuilder().group("vulkan").print("Buffer map already disposed"));
                return;
            }
            if (_map->raw() == nullptr) {
                return;
            }
            _map->dispose();

            bool internal = _internalCommandBuffer != nullptr;
            auto buffer = internal ? _internalCommandBuffer : _externalCommandBuffer;

            if (_range.has_value()) {
                vulkan_util::copyBuffer(buffer->getImplementation().getCommandBuffer(), _stagingBuffer->getRaw(),
                                        _deviceBuffer.getRaw(), _range.value().getFrom(), _range.value().getFrom(),
                                        _range.value().size());
            } else {
                vulkan_util::copyBuffer(buffer->getImplementation().getCommandBuffer(), _stagingBuffer->getRaw(),
                                        _deviceBuffer.getRaw(), _deviceBuffer.size());
            }
            if (internal) {
                // Command buffer is not external.
                // End and submit.
                _internalCommandBuffer->end();
                _internalCommandBuffer->submit();
            }

            _disposed = true;
        }

        void dispose() override
        {
            disposeStagingBuffer();
        }

        bool isValid() override
        {
            return !_disposed;
        }
    };

    class StagingBuffer : public Buffer
    {
        AbstractVKApplication* _application;

        std::vector<std::shared_ptr<SimpleBuffer>> _stagingBuffers;
        SimpleBuffer _deviceBuffer;

        std::optional<std::shared_ptr<BufferMap<char>>> rawMap(const CommandBuffer* commandBuffer = nullptr) override;

        std::optional<std::shared_ptr<BufferMap<char>>> rawMap(Range<uint32_t> range,
                                                               const CommandBuffer* commandBuffer = nullptr) override;

      public:
        StagingBuffer(const StagingBuffer& other) = delete;

        template<class T>
        StagingBuffer(AbstractVKApplication* application, VkBufferUsageFlags usage, const std::vector<T>& data) :
            _application(application),
            _stagingBuffers(),
            _deviceBuffer(_application, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                          data)
        {
            for (uint32_t i = 0; i < _application->getMaxFramesInFlight(); ++i) {
                _stagingBuffers.push_back(std::make_shared<SimpleBuffer>(
                    _application, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, data));
            }

            map<T>();
            // Transfers all data from the staging buffer to the device buffer.
        }

        StagingBuffer(AbstractVKApplication* application, VkBufferUsageFlags usage, uint32_t sizeInBytes);

        ~StagingBuffer() override = default;

        size_t size() const override;

        bool canBeWrittenOn() const override;

        VkBuffer getRaw() const override;

        AbstractVKApplication* getApplication() const override;

        SimpleBuffer& getDeviceBuffer();

        const SimpleBuffer& getDeviceBuffer() const;
    };
} // namespace neon::vulkan

#endif //VULKANTEST_STAGINGBUFFER_H
