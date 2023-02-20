//
// Created by grial on 20/02/23.
//

#include "Node.h"
#include <Eigen/src/Core/util/Meta.h>
#include <Eigen/src/SparseCore/SparseUtil.h>

void Node::initialize(int startIndex) {

}

uint32_t Node::getNumberOfDegreesOfFreedom() const {
    return 3;
}

void Node::getPosition(Eigen::VectorXf& vector) const {
    vector(_doFIndex) = _position(0);
    vector(_doFIndex + 1) = _position(1);
    vector(_doFIndex + 2) = _position(2);
}

void Node::setPosition(Eigen::VectorXf& vector) {
    _position(0) = vector(_doFIndex);
    _position(1) = vector(_doFIndex + 1);
    _position(2) = vector(_doFIndex + 2);
}

void Node::getVelocity(Eigen::VectorXf& vector) const {
    vector(_doFIndex) = _velocity(0);
    vector(_doFIndex + 1) = _velocity(1);
    vector(_doFIndex + 2) = _velocity(2);
}

void Node::setVelocity(Eigen::VectorXf& vector) {
    _velocity(0) = vector(_doFIndex);
    _velocity(1) = vector(_doFIndex + 1);
    _velocity(2) = vector(_doFIndex + 2);
}

void Node::getForce(Eigen::VectorXf& vector) const {
    auto damping = -_damping * _mass * _velocity;
    auto force = Eigen::Vector3f(0.0f, -9.81f, 0.0f) * _mass + damping;

    vector(_doFIndex) += force(0);
    vector(_doFIndex + 1) += force(1);
    vector(_doFIndex + 2) += force(2);
}

void Node::getForceJacobian(std::vector<Eigen::Triplet<float>>& dFdx,
                            std::vector<Eigen::Triplet<float>>& dFdv) const {
    float damping = -_damping * _mass;

    dFdv.emplace_back(_doFIndex, _doFIndex, damping);
    dFdv.emplace_back(_doFIndex + 1, _doFIndex + 1, damping);
    dFdv.emplace_back(_doFIndex + 2, _doFIndex + 2, damping);
}

void Node::getMass(std::vector<Eigen::Triplet<float>>& mass) const {
    mass.emplace_back(_doFIndex, _doFIndex, _mass);
    mass.emplace_back(_doFIndex + 1, _doFIndex + 1, _mass);
    mass.emplace_back(_doFIndex + 2, _doFIndex + 2, _mass);
}

void Node::getInverseMass(std::vector<Eigen::Triplet<float>>& mass) const {
    float inv = 1.0f / _mass;
    mass.emplace_back(_doFIndex, _doFIndex, inv);
    mass.emplace_back(_doFIndex + 1, _doFIndex + 1, inv);
    mass.emplace_back(_doFIndex + 2, _doFIndex + 2, inv);
}

void Node::fixVector(Eigen::VectorXf& vector) const {
    if (_fixed) {
        vector(_doFIndex) = 0.0f;
        vector(_doFIndex + 1) = 0.0f;
        vector(_doFIndex + 2) = 0.0f;
    }
}

void Node::fixMatrix(Eigen::SparseMatrix<float>& matrix) const {
    if (_fixed) {
        int64_t d = _doFIndex;

        matrix.insert(d, d) = 1.0f;
        matrix.insert(d + 1, d + 1) = 1.0f;
        matrix.insert(d + 2, d + 2) = 1.0f;

        matrix.prune([d](const Eigen::Index& row, const Eigen::Index& col,
                         float& value) {

            if (row == d && col == d) {
                return true;
            }

            return (d - row < 0 || d - row >= 3) &&
                   (d - col < 0 || d - col >= 3) || row == col;
        });
    }
}

uint32_t Node::getDoFIndex() const {
    return _doFIndex;
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
