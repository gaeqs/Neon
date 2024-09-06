//
// Created by gaelr on 21/10/2022.
//

#ifndef RVTRACKING_IDENTIFIABLEWRAPPER_H
#define RVTRACKING_IDENTIFIABLEWRAPPER_H

#include <any>
#include <cstdint>
#include <iostream>

#include <neon/structure/Identifiable.h>

namespace neon {
    template<class T>
    class IdentifiableWrapper {

        friend class std::hash<IdentifiableWrapper<T>>;

        T* _pointer;
        uint64_t _id;

    public:

        IdentifiableWrapper() :
                _pointer(nullptr),
                _id(0) {
        }

        IdentifiableWrapper(T* pointer) :
                _pointer(pointer),
                _id(_pointer == nullptr ? 0 : _pointer->_id) {
            if (_id == 0 && _pointer != nullptr) {
                std::cerr
                        << "Invalid identifiable found on wrapper constructor! "
                           "Setting pointer to null" << std::endl;
                _pointer = nullptr;
            }
        }

        template<class O>
        requires std::is_base_of_v<T, O>
        IdentifiableWrapper(const IdentifiableWrapper<O>& other) :
                _pointer(other.raw()),
                _id(other.getId()) {
        }

        inline uint64_t getId() const {
            return _id;
        }

        [[nodiscard]] inline bool isValid() const {
            return _pointer != nullptr && _pointer != (void*) 0xFFFFFFFFFFFFFFFF
                   && _pointer->_id == _id;
        }

        inline T* raw() const {
            return isValid() ? _pointer : nullptr;
        }

        inline T* operator->() const {
            return isValid() ? _pointer : nullptr;
        }

        inline bool operator==(const std::nullptr_t other) const {
            return !isValid();
        }

        inline bool operator!=(const std::nullptr_t other) const {
            return isValid();
        }

        inline bool operator==(const IdentifiableWrapper<T>& other) const {
            return _id == other._id;
        }

        inline bool operator!=(const IdentifiableWrapper<T>& other) const {
            return _id != other._id;
        }

        inline operator bool() const {
            return isValid();
        }

        template<class O>
        requires std::is_base_of_v<T, O>
        IdentifiableWrapper<T>& operator=(const IdentifiableWrapper<O>& other) {
            _pointer = other.raw();
            _id = other.getId();
            return *this;
        }
    };
}

template<class T>
struct std::hash<neon::IdentifiableWrapper<T>> {
    std::size_t operator()(neon::IdentifiableWrapper<T> const& s)
    const noexcept {
        return std::hash<uint64_t>{}(s._id);
    }
};

#endif //RVTRACKING_IDENTIFIABLEWRAPPER_H
