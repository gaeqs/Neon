//
// Created by gaeqs on 02/03/2023.
//

#include "Asset.h"

#include <utility>

namespace neon {

    bool AssetIdentifier::operator==(const AssetIdentifier& other) const {
        return type == other.type && name == other.name;
    }

    bool AssetIdentifier::operator!=(const AssetIdentifier& other) const {
        return type != other.type || name != other.name;
    }

    Asset::Asset(std::type_index type, std::string name) :
            _identifier({std::move(type), std::move(name)}) {
    }

    const AssetIdentifier& Asset::getIdentifier() const {
        return _identifier;
    }

    const std::type_index& Asset::getType() const {
        return _identifier.type;
    }

    const std::string& Asset::getName() const {
        return _identifier.name;
    }

    bool Asset::operator==(const Asset& other) const {
        return _identifier == other._identifier;
    }

    bool Asset::operator!=(const Asset& other) const {
        return _identifier != other._identifier;
    }
}