//
// Created by gaelr on 21/10/2022.
//

#ifndef RVTRACKING_IDENTIFIABLEWRAPPER_H
#define RVTRACKING_IDENTIFIABLEWRAPPER_H

#include <type_traits>
#include <bitset>
#include <functional>

#include <neon/logging/Logger.h>
#include <neon/structure/Identifiable.h>

namespace neon
{
    template<class T>
    class IdentifiableWrapper
    {
        IdentifiableCounter* _counter;

      public:
        IdentifiableWrapper() :
            _counter(nullptr)
        {
        }

        IdentifiableWrapper(T* pointer) :
            _counter(pointer == nullptr ? nullptr : pointer->getCounter())
        {
            if (_counter == nullptr || !_counter->valid) {
                if (pointer != nullptr) {
                    logger.error("Invalid identifiable found on wrapper constructor! "
                                 "Setting pointer to null");
                }
                _counter = nullptr;
            } else {
                ++_counter->counter;
            }
        }

        IdentifiableWrapper(const IdentifiableWrapper& other) :
            _counter(other._counter)
        {
            if (_counter && _counter->valid) {
                ++_counter->counter;
            } else {
                _counter = nullptr;
            }
        }

        template<class O>
            requires std::is_base_of_v<T, O>
        IdentifiableWrapper(const IdentifiableWrapper<O>& other) :
            _counter(other.getCounter())
        {
            if (_counter != nullptr && _counter->valid) {
                ++_counter->counter;
            } else {
                _counter = nullptr;
            }
        }

        ~IdentifiableWrapper()
        {
            if (_counter != nullptr) {
                --_counter->counter;
                if (!_counter->valid && _counter->counter == 0) {
                    delete _counter;
                }
            }
        }

        uint64_t getId() const
        {
            if (_counter == nullptr || !_counter->valid) {
                return 0;
            }
            return _counter->ptr->getId();
        }

        [[nodiscard]] bool isValid() const
        {
            return _counter != nullptr && _counter->valid;
        }

        T* raw() const
        {
            return isValid() ? dynamic_cast<T*>(_counter->ptr) : nullptr;
        }

        T* operator->() const
        {
            return isValid() ? dynamic_cast<T*>(_counter->ptr) : nullptr;
        }

        bool operator==(const std::nullptr_t other) const
        {
            return !isValid();
        }

        bool operator!=(const std::nullptr_t other) const
        {
            return isValid();
        }

        bool operator==(const IdentifiableWrapper<T>& other) const
        {
            return _counter == other._counter;
        }

        bool operator!=(const IdentifiableWrapper<T>& other) const
        {
            return _counter != other._counter;
        }

        operator bool() const
        {
            return isValid();
        }

        IdentifiableCounter* getCounter() const
        {
            return _counter;
        }


        IdentifiableWrapper& operator=(const IdentifiableWrapper& other)
        {
            // Delete data from the old counter
            if (_counter != nullptr) {
                --_counter->counter;
                if (!_counter->valid && _counter->counter == 0) {
                    delete _counter;
                }
            }

            _counter = other.getCounter();
            if (_counter != nullptr && _counter->valid) {
                ++_counter->counter;
            } else {
                _counter = nullptr;
            }

            return *this;
        }

        template<class O>
            requires std::is_base_of_v<T, O>
        IdentifiableWrapper<T>& operator=(const IdentifiableWrapper<O>& other)
        {
            // Delete data from the old counter
            if (_counter != nullptr) {
                --_counter->counter;
                if (!_counter->valid && _counter->counter == 0) {
                    delete _counter;
                }
            }

            _counter = other.getCounter();
            if (_counter != nullptr && _counter->valid) {
                ++_counter->counter;
            } else {
                _counter = nullptr;
            }

            return *this;
        }
    };
} // namespace neon

template<class T>
struct std::hash<neon::IdentifiableWrapper<T>>
{
    std::size_t operator()(const neon::IdentifiableWrapper<T>& s) const noexcept
    {
        auto* c = s.getCounter();
        if (c == nullptr || !c->valid) {
            return 0;
        }
        return std::hash<uint64_t>{}(c->ptr->getId());
    }
};

#endif // RVTRACKING_IDENTIFIABLEWRAPPER_H
