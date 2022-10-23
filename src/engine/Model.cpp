//
// Created by gaelr on 23/10/2022.
//

#include "Model.h"

uint64_t MODEL_ID_GENERATOR = 1;

void Model::assignId() {
    _id = MODEL_ID_GENERATOR++;
}

uint64_t Model::getId() const {
    return _id;
}

const Model::Implementation& Model::getImplementation() const {
    return _implementation;
}

Model::Implementation& Model::getImplementation() {
    return _implementation;
}
