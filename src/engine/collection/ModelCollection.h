//
// Created by grial on 21/10/22.
//

#ifndef RVTRACKING_MODELCOLLECTION_H
#define RVTRACKING_MODELCOLLECTION_H

#include <string>
#include <memory>
#include <optional>
#include <unordered_map>

#include "engine/Model.h"
#include "engine/IdentifiableWrapper.h"
#include "util/ClusteredLinkedCollection.h"

/**
 * This class holds all models of a room.
 */
class ModelCollection {

    ClusteredLinkedCollection<Model> _models;
    std::unordered_map<uint64_t, IdentifiableWrapper<Model>> _modelsById;

public:

    /**
     * Creates the collection.
     */
    ModelCollection();

    /**
     * Creates a new model.
     * @param data the data of the model.
     * @param width the width of the model.
     * @param height the height of the model.
     * @return a pointer to the new model.
     */
    template<class Vertex>
    IdentifiableWrapper<Model> createModel(
            std::vector<Vertex> vertices, std::vector<uint32_t> indices) {
        auto value = IdentifiableWrapper<Model>(
                _models.emplace(vertices, indices));
        _modelsById.emplace(value.isValid(), value);
        return value;
    }

    /**
     * Returns a pointer to the model that matches the given id.
     * @param id the id.
     * @return the pointer to the model.
     */
    IdentifiableWrapper<Model> getModel(uint64_t id) const;

    /**
     * Destroy the given model.
     *
     * All pointers to the given model will be invalid after this call.
     *
     * @param model the model.
     * @return whether the operation was successful or not.
     */
    bool destroyModel(const IdentifiableWrapper<Model>& model);
};


#endif //RVTRACKING_MODELCOLLECTION_H
