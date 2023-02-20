//
// Created by grial on 20/02/23.
//

#include "PhysicsManager.h"
#include "Eigen/src/Core/util/Constants.h"
#include "Eigen/src/SparseCore/SparseMatrix.h"
#include "Eigen/src/SparseLU/SparseLU.h"

#include <imgui.h>

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/SparseLU>
#include <vector>

PhysicsManager::PhysicsManager(IntegrationMode mode) :
        _mode(mode),
        _degreesOfFreedom(0),
        _objects(),
        _paused(true) {

}

void PhysicsManager::onUpdate(float deltaTime) {
    if (_paused || _degreesOfFreedom == 0) return;
    switch (_mode) {
        case IntegrationMode::SYMPLECTIC:
            stepSymplectic(0.01f);
            break;
        case IntegrationMode::IMPLICIT:
            stepImplicit(0.01f);
            break;
    }
}

void PhysicsManager::stepSymplectic(float deltaTime) {
    static Eigen::VectorXf x;
    static Eigen::VectorXf v;
    static Eigen::VectorXf f;
    static Eigen::SparseMatrix<float> massInv;
    static std::vector<Eigen::Triplet<float>> massTriplets;

    x.resize(_degreesOfFreedom);
    v.resize(_degreesOfFreedom);
    f.resize(_degreesOfFreedom);
    massInv.resize(_degreesOfFreedom, _degreesOfFreedom);

    x.fill(0.0f);
    v.fill(0.0f);
    f.fill(0.0f);


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

void PhysicsManager::stepImplicit(float deltaTime) {
    static Eigen::VectorXf x;
    static Eigen::VectorXf v;
    static Eigen::VectorXf f;
    static Eigen::SparseMatrix<float> mass;
    static Eigen::SparseMatrix<float> k;
    static Eigen::SparseMatrix<float> d;
    static std::vector<Eigen::Triplet<float>> massTriplets;
    static std::vector<Eigen::Triplet<float>> kTriplets;
    static std::vector<Eigen::Triplet<float>> dTriplets;

    x.resize(_degreesOfFreedom);
    v.resize(_degreesOfFreedom);
    f.resize(_degreesOfFreedom);
    mass.resize(_degreesOfFreedom, _degreesOfFreedom);
    k.resize(_degreesOfFreedom, _degreesOfFreedom);
    d.resize(_degreesOfFreedom, _degreesOfFreedom);

    massTriplets.clear();
    kTriplets.clear();
    dTriplets.clear();
    massTriplets.reserve(_degreesOfFreedom);
    kTriplets.reserve(_degreesOfFreedom);
    dTriplets.reserve(_degreesOfFreedom);

    x.fill(0.0f);
    v.fill(0.0f);
    f.fill(0.0f);

    for (const auto& obj: _objects) {
        obj->getPosition(x);
        obj->getVelocity(v);
        obj->getForce(f);
        obj->getMass(massTriplets);
        obj->getForceJacobian(kTriplets, dTriplets);
        mass.setFromTriplets(massTriplets.begin(), massTriplets.end());
        k.setFromTriplets(kTriplets.begin(), kTriplets.end());
        d.setFromTriplets(dTriplets.begin(), dTriplets.end());
    }

    Eigen::SparseMatrix<float> a = mass - deltaTime * d
                                   - deltaTime * deltaTime * k;
    Eigen::VectorXf b = (mass - deltaTime * d) * v + deltaTime * f;

    for (const auto& obj: _objects) {
        obj->fixVector(b);
        obj->fixMatrix(a);
    }

    Eigen::SparseLU<Eigen::SparseMatrix<float>> solver;
    solver.analyzePattern(a);
    solver.factorize(a);

    v = solver.solve(b);
    x += deltaTime * v;

    for (const auto& obj: _objects) {
        obj->setPosition(x);
        obj->setVelocity(v);
    }
}

void PhysicsManager::drawEditor() {
    if (ImGui::Button(_paused ? "Resume" : "Pause")) {
        _paused = !_paused;
    }

    if (ImGui::Button(_mode == IntegrationMode::SYMPLECTIC
                      ? "Symplectic" : "Implicit")) {
        _mode = _mode == IntegrationMode::SYMPLECTIC
                ? IntegrationMode::IMPLICIT
                : IntegrationMode::SYMPLECTIC;
    }
}