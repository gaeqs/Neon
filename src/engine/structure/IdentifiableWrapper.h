//
// Created by gaelr on 21/10/2022.
//

#ifndef RVTRACKING_IDENTIFIABLEWRAPPER_H
#define RVTRACKING_IDENTIFIABLEWRAPPER_H

#include <cstdint>
#include <iostream>

template<class T>
class IdentifiableWrapper {

    friend class std::hash<IdentifiableWrapper<T>>;

    T* _pointer;
    uint64_t _componentId;

public:

    IdentifiableWrapper() :
            _pointer(nullptr),
            _componentId(0) {
    }

    IdentifiableWrapper(T* pointer) :
            _pointer(pointer),
            _componentId(_pointer == nullptr ? 0 : _pointer->_id) {
        if (_componentId == 0 && _pointer != nullptr) {
            std::cerr << "Invalid identifiable found on wrapper constructor! "
                         "Setting pointer to null" << std::endl;
            _pointer = nullptr;
        }
    }

    [[nodiscard]] inline bool isValid() const {
        return _pointer != nullptr && _pointer->_id == _componentId;
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
        return _componentId == other._componentId;
    }

    inline bool operator!=(const IdentifiableWrapper<T>& other) const {
        return _componentId != other._componentId;
    }

    inline operator bool() const {
        return isValid();
    }

};

template<class T>
struct std::hash<IdentifiableWrapper<T>> {
    std::size_t operator()(IdentifiableWrapper<T> const& s) const noexcept {
        return std::hash<uint64_t>{}(s._componentId);
    }
};

#endif //RVTRACKING_IDENTIFIABLEWRAPPER_H
