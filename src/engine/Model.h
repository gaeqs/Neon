//
// Created by gaelr on 23/10/2022.
//

#ifndef RVTRACKING_MODEL_H
#define RVTRACKING_MODEL_H

#include <vector>

#include <engine/Identifiable.h>

#ifdef USE_OPENGL

#include <gl/GLModel.h>

#endif

class Model : public Identifiable {

    template<class T> friend
    class IdentifiableWrapper;

#ifdef USE_OPENGL
    using Implementation = GLModel;
#endif

    uint64_t _id;
    Implementation _implementation;

    void assignId();

public:

    Model(const Model& model) = delete;

    template<class Vertex>
    Model(std::vector<Vertex> vertices, std::vector<uint32_t> indices):
            _id(0),
            _implementation(vertices, indices) {
        assignId();
    }

    uint64_t getId() const override;

    const Implementation& getImplementation() const;

    Implementation& getImplementation();

};


#endif //RVTRACKING_MODEL_H
