//
// Created by gaeqs on 02/03/2023.
//

#ifndef NEON_ASSET_H
#define NEON_ASSET_H

#include <functional>
#include <string>
#include <typeindex>

namespace neon {

    struct AssetIdentifier {

        std::type_index type;
        std::string name;

        bool operator==(const AssetIdentifier& other) const;

        bool operator!=(const AssetIdentifier& other) const;
    };

    /**
     * Represents an element with a name that can be
     * shared between rooms.
     * <p>
     * An asset is represented by a type and a name.
     * If two assets have the same type and name, they
     * are considered the same asset.
     * <p>
     * An asset may be, for example, a Shader, a Texture or
     * a Model.
     */
    class Asset {

        AssetIdentifier _identifier;

    public:

        /**
         * Creates an asset
         * @param type the type of the asset.
         * @param name the name of the asset.
         */
        Asset(std::type_index type, std::string name);

        virtual ~Asset() = default;

        /**
         * Returns the type of the asset.
         * @return the type.
         */
        [[nodiscard]] const std::type_index& getType() const;

        /**
         * Returns the identifier of this asset.
         * @return the identifier.
         */
        [[nodiscard]] const AssetIdentifier& getIdentifier() const;

        /**
         * Returns the name of the asset.
         * @return the name.
         */
        [[nodiscard]] const std::string& getName() const;

        /**
         * Returns whether the given asset is equivalent to this asset.
         * @param other the other asset.
         * @return whether the two assets are equivalent.
         */
        bool operator==(const Asset& other) const;

        /**
        * Returns whether the given asset is not equivalent to this asset.
        * @param other the other asset.
        * @return whether the two assets are not equivalent.
        */
        bool operator!=(const Asset& other) const;

    };
}

namespace std {

    template<>
    struct hash<neon::AssetIdentifier> {

        std::size_t operator()(neon::AssetIdentifier const& s) const noexcept {
            std::size_t h1 = std::hash<std::type_index>{}(s.type);
            std::size_t h2 = std::hash<std::string>{}(s.name);
            return h1 ^ (h2 << 1);
        }

    };

}

#endif //NEON_ASSET_H
