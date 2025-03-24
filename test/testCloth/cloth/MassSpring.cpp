//
// Created by grial on 20/02/23.
//

#include "MassSpring.h"
#include "Eigen/src/Core/Matrix.h"

#include <unordered_map>

template<>
struct std::hash<Eigen::Vector2i>
{
    std::size_t operator()(const Eigen::Vector2i& a) const
    {
        return a.x() ^ (a.y() << 1);
    }
};

MassSpring::MassSpring(uint32_t dofIndexStart, neon::IdentifiableWrapper<PhysicsManager> manager,
                       neon::IdentifiableWrapper<neon::GameObject> gameObject, std::vector<rush::Vec3f>& positions,
                       std::vector<uint32_t>& triangles, float mass, float stiffnessStretch, float stiffnessBend,
                       float dampingAlpha, float dampingBeta) :
    _index(dofIndexStart),
    _nodes(),
    _springs(),
    _manager(manager),
    _gameObject(gameObject)
{
    generateNodes(positions, mass, dampingAlpha);
    generateSprings(triangles, dampingBeta, stiffnessStretch, stiffnessBend);
}

void MassSpring::generateNodes(std::vector<rush::Vec3f>& positions, float mass, float dampingAlpha)
{
    _nodes.reserve(positions.size());
    float massPerNode = mass / static_cast<float>(positions.size());

    uint32_t i = _index;
    for (const auto& position : positions) {
        auto pos = _gameObject->getTransform().getModel() * rush::Vec4f(position, 1.0f);

        _nodes.emplace_back(i, massPerNode, dampingAlpha, _index == i || _index + 57 == i,
                            Eigen::Vector3f(pos.x(), pos.y(), pos.z()));

        i += 3;
    }
}

void MassSpring::generateSprings(std::vector<uint32_t>& triangles, float dampingBeta, float stiffnessStretch,
                                 float stiffnessBend)
{
    _springs.reserve(triangles.size() * 4);

    auto usedEdges = std::unordered_map<Eigen::Vector2i, int>();

    Eigen::Vector3i edges[3];
    for (uint32_t i = 0; i < triangles.size(); i += 3) {
        uint32_t a = triangles[i];
        uint32_t b = triangles[i + 1];
        uint32_t c = triangles[i + 2];
        edges[0] = Eigen::Vector3i(a, b, c);
        edges[1] = Eigen::Vector3i(b, c, a);
        edges[2] = Eigen::Vector3i(c, a, b);

        for (const auto& edge : edges) {
            auto it = usedEdges.find(Eigen::Vector2i(edge.x(), edge.y()));
            if (it != usedEdges.end()) {
                // BEND SPRING
                _springs.emplace_back(edge.z(), it->second, SpringType::BEND, stiffnessBend, dampingBeta, _nodes);
            } else {
                // STRETCH SPRING
                usedEdges[Eigen::Vector2i(edge.y(), edge.x())] = edge.z();

                _springs.emplace_back(edge.x(), edge.y(), SpringType::STRETCH, stiffnessStretch, dampingBeta, _nodes);
            }
        }
    }
}

uint32_t MassSpring::getNumberOfDegreesOfFreedom() const
{
    return 3 * _nodes.size();
}

void MassSpring::getPosition(Eigen::VectorXf& vector) const
{
    for (const auto& item : _nodes) {
        item.getPosition(vector);
    }
}

void MassSpring::setPosition(const Eigen::VectorXf& vector)
{
    for (auto& item : _nodes) {
        item.setPosition(vector);
    }
    for (auto& item : _springs) {
        item.updateState(_nodes);
    }
}

void MassSpring::getVelocity(Eigen::VectorXf& vector) const
{
    for (const auto& item : _nodes) {
        item.getVelocity(vector);
    }
}

void MassSpring::setVelocity(const Eigen::VectorXf& vector)
{
    for (auto& item : _nodes) {
        item.setVelocity(vector);
    }
}

void MassSpring::getForce(Eigen::VectorXf& vector) const
{
    for (const auto& item : _nodes) {
        item.getForce(vector);
    }
    for (const auto& item : _springs) {
        item.getForce(vector);
    }
}

void MassSpring::getForceJacobian(std::vector<Eigen::Triplet<float>>& dFdx,
                                  std::vector<Eigen::Triplet<float>>& dFdv) const
{
    for (const auto& item : _nodes) {
        item.getForceJacobian(dFdx, dFdv);
    }
    for (const auto& item : _springs) {
        item.getForceJacobian(dFdx, dFdv);
    }
}

void MassSpring::getMass(std::vector<Eigen::Triplet<float>>& mass) const
{
    for (const auto& item : _nodes) {
        item.getMass(mass);
    }
}

void MassSpring::getInverseMass(std::vector<Eigen::Triplet<float>>& mass) const
{
    for (const auto& item : _nodes) {
        item.getInverseMass(mass);
    }
}

void MassSpring::fixVector(Eigen::VectorXf& vector) const
{
    for (const auto& item : _nodes) {
        item.fixVector(vector);
    }
}

void MassSpring::fixMatrix(Eigen::SparseMatrix<float>& matrix) const
{
    for (const auto& item : _nodes) {
        item.fixMatrix(matrix);
    }
}

const std::vector<Node>& MassSpring::getNodes() const
{
    return _nodes;
}
