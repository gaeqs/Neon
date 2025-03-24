//
// Created by grial on 17/10/22.
//

#ifndef VULKANTEST_BUFFER_H
#define VULKANTEST_BUFFER_H

#include <optional>
#include <memory>

#include <vulkan/vulkan.h>
#include <neon/util/Range.h>

namespace neon
{
    class CommandBuffer;
}

namespace neon::vulkan
{
    class AbstractVKApplication;

    template<class T>
    class BufferMap
    {
      public:
        BufferMap() = default;

        BufferMap(const BufferMap& other) = delete;

        BufferMap(BufferMap&& other) = default;

        virtual ~BufferMap() = default;

        virtual T& operator[](size_t index) = 0;

        virtual T operator[](size_t index) const = 0;

        virtual T* raw() = 0;

        virtual void dispose() = 0;

        virtual bool isValid() = 0;

        inline T& get(size_t index)
        {
            return raw()[index];
        }

        inline T get(size_t index) const
        {
            return raw()[index];
        }
    };

    class Buffer
    {
        virtual std::optional<std::shared_ptr<BufferMap<char>>> rawMap(
            const CommandBuffer* commandBuffer = nullptr) = 0;

        virtual std::optional<std::shared_ptr<BufferMap<char>>> rawMap(
            Range<uint32_t> range, const CommandBuffer* commandBuffer = nullptr) = 0;

      public:
        Buffer() = default;

        Buffer(const Buffer& other) = delete;

        virtual ~Buffer() = default;

        virtual size_t size() const = 0;

        virtual bool canBeWrittenOn() const = 0;

        virtual VkBuffer getRaw() const = 0;

        virtual AbstractVKApplication* getApplication() const = 0;

        template<class T>
        std::optional<std::shared_ptr<BufferMap<T>>> map(const CommandBuffer* commandBuffer = nullptr)
        {
            auto optional = rawMap(commandBuffer);
            if (optional.has_value()) {
                return std::reinterpret_pointer_cast<BufferMap<T>>(optional.value());
            }
            return {};
        }

        template<class T>
        std::optional<std::shared_ptr<BufferMap<T>>> map(Range<uint32_t> range,
                                                         const CommandBuffer* commandBuffer = nullptr)
        {
            auto optional = rawMap(range, commandBuffer);
            if (optional.has_value()) {
                return std::reinterpret_pointer_cast<BufferMap<T>>(optional.value());
            }
            return {};
        }
    };
} // namespace neon::vulkan

#endif //VULKANTEST_BUFFER_H
