//
// Created by grial on 20/02/23.
//

#include "Spring.h"
#include <Eigen/src/Core/Matrix.h>

Spring::Spring(uint32_t a, uint32_t b, SpringType type, float stiffness, float damping,
               const std::vector<Node>& nodes) :
    _nodeA(a),
    _nodeB(b),
    _nodeADoFIndex(nodes[a].getDoFIndex()),
    _nodeBDoFIndex(nodes[b].getDoFIndex()),
    _type(type),
    _stiffness(stiffness),
    _damping(damping),
    _length0(0.0f),
    _length(0.0f),
    _direction(),
    _velocityBA()
{
    _direction = nodes[a].getPositionVector() - nodes[b].getPositionVector();
    _length0 = _direction.norm();
    _length = _length0;
    _direction /= _length0;
    _velocityBA = nodes[a].getVelocityVector() - nodes[b].getVelocityVector();
}

void Spring::updateState(const std::vector<Node>& nodes)
{
    _direction = nodes[_nodeA].getPositionVector() - nodes[_nodeB].getPositionVector();
    _length = _direction.norm();
    _direction /= _length;
    _velocityBA = nodes[_nodeA].getVelocityVector() - nodes[_nodeB].getVelocityVector();
}

void Spring::getForce(Eigen::VectorXf& vector) const
{
    Eigen::Vector3f dampingA = -_damping * _stiffness * _velocityBA.dot(_direction) * _direction;
    Eigen::Vector3f springA = -_stiffness * (_length - _length0) * _direction;
    Eigen::Vector3f forceA = springA;
    vector(_nodeADoFIndex) += forceA(0);
    vector(_nodeADoFIndex + 1) += forceA(1);
    vector(_nodeADoFIndex + 2) += forceA(2);
    vector(_nodeBDoFIndex) -= forceA(0);
    vector(_nodeBDoFIndex + 1) -= forceA(1);
    vector(_nodeBDoFIndex + 2) -= forceA(2);
}

void Spring::getForceJacobian(std::vector<Eigen::Triplet<float>>& dFdx, std::vector<Eigen::Triplet<float>>& dFdv) const
{
    Eigen::Matrix3f uM = _direction * _direction.transpose();
    Eigen::Matrix3f identity = Eigen::Matrix3f::Identity();

    Eigen::Matrix3f first = -_stiffness * (_length - _length0) / _length * identity;
    Eigen::Matrix3f second = -_stiffness * _length0 / _length * uM;
    Eigen::Matrix3f ka = first + second;
    Eigen::Matrix3f da = _damping * _stiffness * uM;

    const auto a = _nodeADoFIndex;
    const auto b = _nodeBDoFIndex;

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            dFdx.emplace_back(a + i, a + j, ka(i, j));
            dFdx.emplace_back(b + i, b + j, ka(i, j));
            dFdx.emplace_back(a + i, b + j, -ka(i, j));
            dFdx.emplace_back(b + i, a + j, -ka(i, j));
            dFdv.emplace_back(a + i, a + j, da(i, j));
            dFdv.emplace_back(b + i, b + j, da(i, j));
            dFdv.emplace_back(a + i, b + j, -da(i, j));
            dFdv.emplace_back(b + i, a + j, -da(i, j));
        }
    }
}
