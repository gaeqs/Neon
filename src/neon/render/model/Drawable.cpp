//
// Created by gaeqs on 29/09/2024.
//

#include "Drawable.h"

namespace neon {
    Drawable::Drawable(std::type_index type, std::string name)
        : Asset(std::move(type), std::move(name)) {}

    const std::unordered_set<std::shared_ptr<Material>>& Drawable::
    getMaterials() const {
        return _materials;
    }

    std::unordered_set<std::shared_ptr<Material>>& Drawable::getMaterials() {
        return _materials;
    }

    void Drawable::setMaterial(const std::shared_ptr<Material>& material) {
        _materials.clear();
        _materials.insert(material);
    }
}
