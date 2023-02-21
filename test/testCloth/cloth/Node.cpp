//
// Created by grial on 20/02/23.
//

#include "Node.h"
#include <Eigen/src/Core/Matrix.h>
#include <Eigen/src/Core/util/Meta.h>
#include <Eigen/src/SparseCore/SparseUtil.h>

#include <iostream>

#include <utility>


uint32_t Node::getNumberOfDegreesOfFreedom() const {
    return 3;
}

void Node::getPosition(Eigen::VectorXf& vector) const {
    vector(_dofIndex) = _position(0);
    vector(_dofIndex + 1) = _position(1);
    vector(_dofIndex + 2) = _position(2);
}

void Node::setPosition(const Eigen::VectorXf& vector) {
    _position(0) = vector(_dofIndex);
    _position(1) = vector(_dofIndex + 1);
    _position(2) = vector(_dofIndex + 2);
}

void Node::getVelocity(Eigen::VectorXf& vector) const {
    vector(_dofIndex) = _velocity(0);
    vector(_dofIndex + 1) = _velocity(1);
    vector(_dofIndex + 2) = _velocity(2);
}

void Node::setVelocity(const Eigen::VectorXf& vector) {
    _velocity(0) = vector(_dofIndex);
    _velocity(1) = vector(_dofIndex + 1);
    _velocity(2) = vector(_dofIndex + 2);
}

void Node::getForce(Eigen::VectorXf& vector) const {
    Eigen::Vector3f damping = -_damping * _mass * _velocity;
    Eigen::Vector3f force = Eigen::Vector3f(0.0f, -9.81f, 0.0f) * _mass
                            + damping;

    vector(_dofIndex) += force(0);
    vector(_dofIndex + 1) += force(1);
    vector(_dofIndex + 2) += force(2);
}

void Node::getForceJacobian(std::vector<Eigen::Triplet<float>>& dFdx,
                            std::vector<Eigen::Triplet<float>>& dFdv) const {
    float damping = -_damping * _mass;

    dFdv.emplace_back(_dofIndex, _dofIndex, damping);
    dFdv.emplace_back(_dofIndex + 1, _dofIndex + 1, damping);
    dFdv.emplace_back(_dofIndex + 2, _dofIndex + 2, damping);
}

void Node::getMass(std::vector<Eigen::Triplet<float>>& mass) const {
    mass.emplace_back(_dofIndex, _dofIndex, _mass);
    mass.emplace_back(_dofIndex + 1, _dofIndex + 1, _mass);
    mass.emplace_back(_dofIndex + 2, _dofIndex + 2, _mass);
}

void Node::getInverseMass(std::vector<Eigen::Triplet<float>>& mass) const {
    float inv = 1.0f / _mass;
    mass.emplace_back(_dofIndex, _dofIndex, inv);
    mass.emplace_back(_dofIndex + 1, _dofIndex + 1, inv);
    mass.emplace_back(_dofIndex + 2, _dofIndex + 2, inv);
}

void Node::fixVector(Eigen::VectorXf& vector) const {
    if (_fixed) {
        vector(_dofIndex) = 0.0f;
        vector(_dofIndex + 1) = 0.0f;
        vector(_dofIndex + 2) = 0.0f;
    }
}

void Node::fixMatrix(Eigen::SparseMatrix<float>& matrix) const {
    if (_fixed) {
        int64_t d = _dofIndex;

        matrix.coeffRef(d, d) = 1.0f;
        matrix.coeffRef(d + 1, d + 1) = 1.0f;
        matrix.coeffRef(d + 2, d + 2) = 1.0f;

        matrix.prune([d](const Eigen::Index& row, const Eigen::Index& col,
                         const float& value) {

            if (row == d && col == d) {
                return true;
            }

            return (d < row || d + 3 >= row) &&
                   (d < col || d + 3 >= col) || row == col;
        });
    }
}

uint32_t Node::getDoFIndex() const {
    return _dofIndex;
}

float Node::getMassAsFloat() const {
    return _mass;
}

bool Node::isFixed() const {
    return _fixed;
}

const Eigen::Vector3f& Node::getPositionVector() const {
    return _position;
}

const Eigen::Vector3f& Node::getVelocityVector() const {
    return _velocity;
}

Node::Node(uint32_t doFIndex,
           float mass,
           float damping,
           bool fixed,
           Eigen::Vector3f position) :
        _dofIndex(doFIndex),
        _mass(mass),
        _damping(damping),
        _fixed(fixed),
        _position(std::move(position)),
        _velocity(0.0f, 0.0f, 0.0f) {}
