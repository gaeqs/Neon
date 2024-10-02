//
// Created by gaeqs on 02/10/2024.
//

#include "MeshShaderDrawable.h"

namespace neon {
    MeshShaderDrawable::MeshShaderDrawable(
        Application* application,
        const std::string& name,
        std::shared_ptr<Material> material)
        : Drawable(typeid(MeshShaderDrawable), name),
          _implementation(application) {
        getMaterials().insert(std::move(material));
    }


    MeshShaderDrawable::MeshShaderDrawable(
        Application* application,
        const std::string& name,
        const std::unordered_set<std::shared_ptr<Material>>& materials)
        : Drawable(typeid(MeshShaderDrawable), name),
          _implementation(application) {
        getMaterials().insert(materials.begin(), materials.end());
    }

    MeshShaderDrawable::Implementation& MeshShaderDrawable::
    getImplementation() {
        return _implementation;
    }

    const MeshShaderDrawable::Implementation& MeshShaderDrawable::
    getImplementation() const {
        return _implementation;
    }

    void MeshShaderDrawable::setGroups(rush::Vec<3, uint32_t> dimensions) {
        setGroupsSupplier([dimensions](auto&) {
            return dimensions;
        });
    }

    void MeshShaderDrawable::setGroupsSupplier(
        const std::function<rush::Vec<3, uint32_t>(const Model&)>& supplier) {
        _implementation.setGroupsSupplier(supplier);
    }
}
