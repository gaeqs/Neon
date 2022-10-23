//
// Created by grial on 21/10/22.
//

#include "ModelCollection.h"

ModelCollection::ModelCollection() : _models() {
}

IdentifiableWrapper<Model>
ModelCollection::getModel(uint64_t id) const {
    auto it = _modelsById.find(id);
    if (it != _modelsById.end()) {
        return it->second;
    }
    return nullptr;
}

bool ModelCollection::destroyModel(
        const IdentifiableWrapper<Model>& model) {
    _modelsById.erase(model->getId());
    return _models.remove(model.raw());
}
