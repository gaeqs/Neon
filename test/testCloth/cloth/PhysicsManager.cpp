//
// Created by grial on 20/02/23.
//

#include "PhysicsManager.h"

#include <algorithm>
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

    float time = std::min(deltaTime, 0.05f);

    switch (_mode) {
        case IntegrationMode::SYMPLECTIC:
            stepSymplectic(time);
            break;
        case IntegrationMode::IMPLICIT:
            stepImplicit(time);
            break;
    }
}

void PhysicsManager::stepSymplectic(float deltaTime) {
    static Eigen::VectorXf x;
    static Eigen::VectorXf v;
    static Eigen::VectorXf f;
    static Eigen::SparseMatrix<float> massInv;
    static std::vector<Eigen::Triplet<float>> massTriplets;

    {
        DEBUG_PROFILE_ID(getRoom()->getApplication()->getProfiler(),
                         sympletic_initialization,
                         "Symplectic initialization");
        x.resize(_degreesOfFreedom);
        v.resize(_degreesOfFreedom);
        f.resize(_degreesOfFreedom);
        massInv.resize(_degreesOfFreedom, _degreesOfFreedom);

        x.fill(0.0f);
        v.fill(0.0f);
        f.fill(0.0f);


        massTriplets.clear();
        massTriplets.reserve(_degreesOfFreedom);
    }
    {
        DEBUG_PROFILE_ID(getRoom()->getApplication()->getProfiler(),
                         sympletic_fetch,
                         "Symplectic fetch");
        for (const auto& obj: _objects) {
            obj->getPosition(x);
            obj->getVelocity(v);
            obj->getForce(f);
            obj->getInverseMass(massTriplets);
            massInv.setFromTriplets(massTriplets.begin(), massTriplets.end());
        }
    }

    {
        DEBUG_PROFILE_ID(getRoom()->getApplication()->getProfiler(),
                         sympletic_fix,
                         "Symplectic fix");
        for (const auto& obj: _objects) {
            obj->fixVector(f);
            obj->fixMatrix(massInv);
        }
    }

    {
        DEBUG_PROFILE_ID(getRoom()->getApplication()->getProfiler(),
                         sympletic_solve,
                         "Symplectic solve");
        v += deltaTime * (massInv * f);
        x += deltaTime * v;
    }

    {
        DEBUG_PROFILE_ID(getRoom()->getApplication()->getProfiler(),
                         sympletic_set,
                         "Symplectic set");
        for (const auto& obj: _objects) {
            obj->setPosition(x);
            obj->setVelocity(v);
        }
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

    {
        DEBUG_PROFILE_ID(getRoom()->getApplication()->getProfiler(),
                         implicit_initialization,
                         "Implicit initialization");

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

    }

    {
        DEBUG_PROFILE_ID(getRoom()->getApplication()->getProfiler(),
                         implicit_fetch,
                         "Implicit fetch");
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
    }

    Eigen::SparseMatrix<float> a;
    Eigen::VectorXf b;

    {
        DEBUG_PROFILE_ID(getRoom()->getApplication()->getProfiler(),
                         implicit_compute_ab,
                         "Implicit compute AB");
        a = mass - deltaTime * d - deltaTime * deltaTime * k;
        b = (mass - deltaTime * d) * v + deltaTime * f;
    }

    {
        DEBUG_PROFILE_ID(getRoom()->getApplication()->getProfiler(),
                         implicit_fix,
                         "Implicit fix");
        for (const auto& obj: _objects) {
            obj->fixVector(b);
            obj->fixMatrix(a);
        }
    }

    {
        DEBUG_PROFILE_ID(getRoom()->getApplication()->getProfiler(),
                         implicit_solve,
                         "Implicit solve");

        Eigen::SparseLU<Eigen::SparseMatrix<float>> solver;
        solver.analyzePattern(a);
        solver.factorize(a);

        v = solver.solve(b);
        x += deltaTime * v;
    }

    {
        DEBUG_PROFILE_ID(getRoom()->getApplication()->getProfiler(),
                         implicit_set,
                         "Implicit set");
        for (const auto& obj: _objects) {
            obj->setPosition(x);
            obj->setVelocity(v);
        }
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