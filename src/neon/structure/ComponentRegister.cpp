//
// Created by gaelr on 27/01/2023.
//

#include "ComponentRegister.h"

#include <utility>

namespace neon {
    ComponentRegister::ComponentRegister() :
        _ids(),
        _entries() {}

    const std::unordered_set<std::type_index>& ComponentRegister::getComponents() const {
        return _ids;
    }

    const std::unordered_map<std::type_index, ComponentRegisterEntry>& ComponentRegister::getEntries() const {
        return _entries;
    }

    std::optional<ComponentRegisterEntry>
    ComponentRegister::getEntry(const std::type_index& type) {
        auto it = _entries.find(type);
        if (it == _entries.end()) {
            return {};
        }
        return it->second;
    }

    std::optional<ComponentRegisterEntry> ComponentRegister::getEntry(std::string name) {
        auto it = std::find_if(
            _entries.begin(),
            _entries.end(),
            [&name](const std::pair<const std::type_index, ComponentRegisterEntry>& entry) {
                return entry.second.name == name;
            }
        );

        if (it == _entries.end()) return {};
        return it->second;
    }

    ComponentRegisterEntry::ComponentRegisterEntry(
        const std::type_index& type,
        std::string name,
        const std::optional<Creator>& creator,
        const std::optional<JsonCreator>& jsonCreator)
        : type(type),
          name(std::move(name)),
          creator(creator),
          jsonCreator(jsonCreator) {}
}
