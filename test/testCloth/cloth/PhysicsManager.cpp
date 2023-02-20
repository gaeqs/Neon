//
// Created by grial on 20/02/23.
//

#include "PhysicsManager.h"

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <vector>

PhysicsManager::PhysicsManager(IntegrationMode mode) :
        _mode(mode),
        _degreesOfFreedom(0),
        _objects() {

}

void PhysicsManager::onUpdate(float deltaTime) {
    if (_degreesOfFreedom = 0) return;
    switch (_mode) {
        case IntegrationMode::SYMPLETIC:
            stepSympletic(deltaTime);
            return;
        case IntegrationMode::IMPLICIT:
            break;
    }
}

void PhysicsManager::stepSympletic(float deltaTime) {
    static Eigen::VectorXf x;
    static Eigen::VectorXf v;
    static Eigen::VectorXf f;
    static Eigen::SparseMatrix<float> massInv;
    static std::vector<Eigen::Triplet<float>> massTriplets;

    x.resize(_degreesOfFreedom);
    v.resize(_degreesOfFreedom);
    f.resize(_degreesOfFreedom);

    massInv.resize(_degreesOfFreedom, _degreesOfFreedom);

    massTriplets.clear();
    massTriplets.reserve(_degreesOfFreedom);


    for (const auto& obj: _objects) {
        obj->getPosition(x);
        obj->getVelocity(v);
        obj->getForce(f);
        obj->getInverseMass(massTriplets);
        massInv.setFromTriplets(massTriplets.begin(), massTriplets.end());
    }

    for (const auto& obj: _objects) {
        obj->fixVector(f);
        obj->fixMatrix(massInv);
    }

    v += deltaTime * (massInv * f);
    x += deltaTime * v;

    for (const auto& obj: _objects) {
        obj->setPosition(x);
        obj->setVelocity(v);
    }
}
