//
// Created by grial on 17/10/22.
//

#ifndef VULKANTEST_BUFFER_H
#define VULKANTEST_BUFFER_H

#include <optional>
#include <memory>

#include <vulkan/vulkan.h>
#include <util/Range.h>

namespace neon::vulkan {

    class VKApplication;

    template<class T>
    class BufferMap {

    public:

        BufferMap() = default;

        BufferMap(const BufferMap& other) = delete;

        virtual ~BufferMap() = default;

        virtual T& operator[](size_t index) = 0;

        virtual T operator[](size_t index) const = 0;

        virtual T* raw() = 0;

        virtual void dispose() = 0;

        inline T& get(size_t index) {
            return raw()[index];
        }

        inline T get(size_t index) const {
            return raw()[index];
        }

    };

    class Buffer {

        virtual std::optional<std::shared_ptr<BufferMap<char>>> rawMap() = 0;

        virtual std::optional<std::shared_ptr<BufferMap<char>>> rawMap(
                Range<uint32_t> range) = 0;

    public:

        Buffer() = default;

        Buffer(const Buffer& other) = delete;

        virtual ~Buffer() = default;

        virtual size_t size() const = 0;

        virtual bool canBeWrittenOn() const = 0;

        virtual VkBuffer getRaw() const = 0;

        virtual VKApplication* getApplication() const = 0;

        template<class T>
        std::optional<std::shared_ptr<BufferMap<T>>> map() {
            auto optional = rawMap();
            if (optional.has_value()) {
                return std::reinterpret_pointer_cast<BufferMap<T>>(
                        optional.value());
            }
            return {};
        }

        template<class T>
        std::optional<std::shared_ptr<BufferMap<T>>>
        map(Range<uint32_t> range) {
            auto optional = rawMap(range);
            if (optional.has_value()) {
                return std::reinterpret_pointer_cast<BufferMap<T>>(
                        optional.value());
            }
            return {};
        }

    };
}

#endif //VULKANTEST_BUFFER_H
