//
// Created by gaelr on 21/10/2022.
//

#ifndef RVTRACKING_COMPONENTWRAPPER_H
#define RVTRACKING_COMPONENTWRAPPER_H

#include <cstdint>

template<class T>
class ComponentWrapper {

    T* _component;
    uint64_t _componentId;

public:

    explicit ComponentWrapper(T* component) :
            _component(component), _componentId(_component->_id) {
    }

    inline bool isValid() const {
        return _component != nullptr && _component->_id == _componentId;
    }

    inline T* operator->() const {
        return isValid() ? _component : nullptr;
    }

    inline T* raw() const {
        return isValid() ? _component : nullptr;
    }

};

#endif //RVTRACKING_COMPONENTWRAPPER_H
