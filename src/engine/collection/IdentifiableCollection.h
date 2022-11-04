//
// Created by gaelr on 04/11/2022.
//

#ifndef NEON_IDENTIFIABLECOLLECTION_H
#define NEON_IDENTIFIABLECOLLECTION_H

#include <type_traits>
#include <unordered_map>

#include <engine/Identifiable.h>
#include <engine/IdentifiableWrapper.h>
#include <util/ClusteredLinkedCollection.h>

template<class T> requires std::is_base_of_v<Identifiable, T>
class IdentifiableCollection {

    ClusteredLinkedCollection<T> _cluster;
    std::unordered_map<uint64_t, IdentifiableWrapper<T>> _idMap;

public:

    template<class... Args>
    IdentifiableWrapper<T> create(Args&& ... values) {
        IdentifiableWrapper<T> ptr = _cluster.emplace(values...);
        _idMap[ptr->getId()] = ptr;
        return ptr;
    }

    IdentifiableWrapper<T> get(uint64_t id) const {
        auto it = _idMap.find(id);
        if (it != _idMap.end()) {
            return it->second;
        }
        return nullptr;
    }

    bool destroy(IdentifiableWrapper<T>& value) {
        _idMap.erase(value->getId());
        return _cluster.remove(value.raw());
    }

    void forEach(std::function<void(T*)> function) {
        _cluster.forEach(function);
    }

};

#endif //NEON_IDENTIFIABLECOLLECTION_H
