//
// Created by gaelr on 27/01/2023.
//

#ifndef NEON_COMPONENTREGISTER_H
#define NEON_COMPONENTREGISTER_H

#include <any>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <neon/loader/AssetLoader.h>

#include <neon/structure/GameObject.h>
#include <nlohmann/json.hpp>

namespace neon
{
    class Component;

    struct ComponentRegisterEntry
    {
        using Creator = std::function<IdentifiableWrapper<Component>(GameObject&)>;

        using JsonCreator =
            std::function<IdentifiableWrapper<Component>(GameObject&, nlohmann::json, AssetLoaderContext context)>;

        std::type_index type;
        std::string name;
        std::optional<Creator> creator;
        std::optional<JsonCreator> jsonCreator;

        ComponentRegisterEntry(const std::type_index& type, std::string name, const std::optional<Creator>& creator,
                               const std::optional<JsonCreator>& jsonCreator);
    };

    class ComponentRegister
    {
        using Creator = std::function<IdentifiableWrapper<Component>(GameObject&)>;

        using JsonCreator =
            std::function<IdentifiableWrapper<Component>(GameObject&, nlohmann::json, AssetLoaderContext context)>;

        std::unordered_set<std::type_index> _ids;
        std::unordered_map<std::type_index, ComponentRegisterEntry> _entries;

        ComponentRegister();

      public:
        static ComponentRegister& instance()
        {
            static ComponentRegister instance = ComponentRegister();
            return instance;
        }

        const std::unordered_set<std::type_index>& getComponents() const;

        const std::unordered_map<std::type_index, ComponentRegisterEntry>& getEntries() const;

        std::optional<ComponentRegisterEntry> getEntry(const std::type_index& type);

        std::optional<ComponentRegisterEntry> getEntry(std::string name);

        template<class T>
            requires std::is_base_of_v<Component, T>
        void registerComponent(const std::string& name)
        {
            std::optional<Creator> creator = {};
            std::optional<JsonCreator> jsonCreator = {};
            if constexpr (std::is_constructible_v<T>) {
                creator = [](GameObject& gameObject) { return gameObject.newComponent<T>(); };
            }
            if constexpr (std::is_constructible_v<T, nlohmann::json, AssetLoaderContext>) {
                jsonCreator = [](GameObject& gameObject, nlohmann::json json, AssetLoaderContext context) {
                    return gameObject.newComponent<T>(std::move(json), context);
                };
            }

            _ids.insert(typeid(T));
            _entries.insert({
                typeid(T), {typeid(T), name, creator, jsonCreator}
            });
        }
    };
} // namespace neon

#endif //NEON_COMPONENTREGISTER_H
