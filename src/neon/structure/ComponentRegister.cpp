//
// Created by gaelr on 27/01/2023.
//

#include "ComponentRegister.h"

#include <utility>

namespace neon {
    ComponentRegister::ComponentRegister() :
            _ids(),
            _entries() {
    }

    const std::unordered_set<std::type_index>&
    ComponentRegister::getComponents() {
        return _ids;
    }

    std::optional<ComponentRegisterEntry>
    ComponentRegister::getEntry(const std::type_index& type) {
        auto it = _entries.find(type);
        if (it == _entries.end()) {
            return {};
        }
        return it->second;
    }

    ComponentRegisterEntry::ComponentRegisterEntry(
            const std::type_index& type,
            std::string name,
            const std::optional<Creator>& creator)
            : type(type), name(std::move(name)), creator(creator) {}
}