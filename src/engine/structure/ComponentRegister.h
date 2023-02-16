//
// Created by gaelr on 27/01/2023.
//

#ifndef NEON_COMPONENTREGISTER_H
#define NEON_COMPONENTREGISTER_H

#include <any>
#include <string>
#include <unordered_set>
#include <unordered_map>

#include <engine/structure/GameObject.h>

namespace neon {
    class Component;

    struct ComponentRegisterEntry {
        using Creator = std::function<IdentifiableWrapper<Component>(
                GameObject&)>;

        std::type_index type;
        std::string name;
        std::optional<Creator> creator;

        ComponentRegisterEntry(
                const std::type_index& type,
                const std::string ame,
                const std::optional<Creator>& creator);
    };

    class ComponentRegister {

        using Creator = std::function<IdentifiableWrapper<Component>(
                GameObject&)>;

        std::unordered_set<std::type_index> _ids;
        std::unordered_map<std::type_index, ComponentRegisterEntry> _entries;

        ComponentRegister();

    public:

        static ComponentRegister& instance() {
            static ComponentRegister instance = ComponentRegister();
            return instance;
        }

        const std::unordered_set<std::type_index>& getComponents();

        std::optional<ComponentRegisterEntry>
        getEntry(const std::type_index& type);

        template<class T>
        requires std::is_base_of_v<Component, T> &&
                 std::is_constructible<T>::value
        void registerComponent(const std::string& name) {
            Creator creator = [](GameObject& gameObject) {
                return gameObject.newComponent<T>();
            };

            _ids.insert(typeid(T));
            _entries.insert({typeid(T), {typeid(T), name, creator}});
        }

        template<class T>
        requires std::is_base_of_v<Component, T> &&
                 (!std::is_constructible<T>::value)
        void registerComponent(const std::string& name) {
            _ids.insert(typeid(T));
            _entries.insert({typeid(T), {typeid(T), name, {}}});
        }

    };
}


#endif //NEON_COMPONENTREGISTER_H
