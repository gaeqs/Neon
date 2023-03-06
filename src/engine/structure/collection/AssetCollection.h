//
// Created by gaeqs on 02/03/2023.
//

#ifndef NEON_ASSETCOLLECTION_H
#define NEON_ASSETCOLLECTION_H

#include <memory>
#include <optional>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include <engine/structure/Asset.h>

namespace neon {

    /**
     * The mode the collection will store an asset.
     */
    enum class AssetStorageMode {

        /**
         * The collection stores a shared
         * pointer of the asset, making it
         * available even if there's no other references.
         */
        PERMANENT,

        /**
         * Weak mode: the collection stores a weak
         * pointer of the asset, releasing it
         * when all other references to the asset are destroyed.
         */
        WEAK
    };

    /**
     * A container for all assets inside an application.
     *
     * Assets can be stored in two possible ways:
     * <ul>
     *  <li>
     *      Permanent mode: the collection stores a shared
     *      pointer of the asset, making it
     *      available even if there's no other references.
     *  </li>
     *  <li>
     *      Weak mode: the collection stores a weak
     *      pointer of the asset, releasing it
     *      when all other references to the asset are destroyed.
     *  </li>
     * </ul>
     */
    class AssetCollection {

        std::unordered_map<AssetIdentifier, std::shared_ptr<Asset>> _permanentAssets;
        std::unordered_map<std::type_index,
                std::unordered_map<std::string, std::weak_ptr<Asset>>> _assets;

    public:

        /**
         * Returns a dictionary containing all assets of the given type.
         * <p>
         * The given collection may contain expired pointers.
         * Use flushExpiredReferences() before calling this method
         * if you don't want to handle them.
         *
         * @tparam Type the type.
         * @return the dictionary.
         */
        template<class Type>
        [[nodiscard]]
        const std::unordered_map<std::string, std::weak_ptr<Asset>>& getAll() const {
            auto it = _assets.find(typeid(Type));
            if (it == _assets.end()) {
                static std::unordered_map<std::string, std::weak_ptr<Asset>> empty;
                return empty;
            }
            return it->second;
        }

        /**
         * Returns the asset of the given type that matches the given name.
         * If the asset is not found, this method returns an empty optional.
         * This method never returns an optional with a null pointer.
         *
         * @tparam Type the type of the asset.
         * @param name the name of the asset.
         * @return the asset or an empty optional.
         */
        template<class Type>
        [[nodiscard]] std::optional<std::shared_ptr<Type>>
        get(const std::string& name) const {
            auto itType = _assets.find(typeid(Type));
            if (itType == _assets.end()) return {};

            auto it = itType->second.find(name);
            if (it == itType->second.end() || it->second.expired()) return {};

            if (auto final = std::dynamic_pointer_cast<Type>(it->second.lock()))
                return final;

            return {};
        }

        /**
         * Returns the asset that matches the given identifier.
         * If the asset is not found, this method returns an empty optional.
         * This method never returns an optional with a null pointer.
         *
         * @param identifier the identifier to match.
         * @return the asset or an empty optional.
         */
        [[nodiscard]] std::optional<std::shared_ptr<Asset>>
        get(const AssetIdentifier& identifier) const;

        /**
         * Adds the given asset to the collection.
         * An asset can be stored in two possible ways:
         * <ul>
         *  <li>
         *      Permanent mode: the collection stores a shared
         *      pointer of the asset, making it
         *      available even if there's no other references.
         *  </li>
         *  <li>
         *      Weak mode: the collection stores a weak
         *      pointer of the asset, releasing it
         *      when all other references to the asset are destroyed.
         *  </li>
         * </ul>
         *
         * This method will override the asset any previously stored asset that
         * shares the same identifier with the given asset.
         *
         * @param asset the asset to store.
         * @param mode the storage mode.
         */
        void store(const std::shared_ptr<Asset>& asset, AssetStorageMode mode);

        /**
         * Removes the asset that matches the given
         * identifier from the collection.
         * <p>
         * This method doesn't deletes the asset: it may still
         * exist in memory if there's any shared pointer addressing it.
         *
         * @param identifier the identifier.
         * @return whether the asset was removed.
         */
        bool remove(const AssetIdentifier& identifier);

        /**
         * Removes the given asset from the collection.
         * <p>
         * This method doesn't deletes the asset: it may still
         * exist in memory if there's any shared pointer addressing it.
         *
         * @param asset the asset to remove.
         * @return whether the asset was removed.
         */
        bool remove(const std::shared_ptr<Asset>& asset);

        /**
         * Removes all expired weak pointers from the collection.
         */
        void flushExpiredReferences();

    };
}

#endif //NEON_ASSETCOLLECTION_H
