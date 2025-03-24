//
// Created by grial on 20/02/23.
//

#ifndef NEON_MASSSPRING_H
#define NEON_MASSSPRING_H

#include <cstdint>
#include <vector>
#include <rush/rush.h>

#include <Eigen/Sparse>
#include <Eigen/Sparse>
#include <Eigen/Dense>

#include <neon/Neon.h>

#include "ISimulable.h"
#include "Node.h"
#include "PhysicsManager.h"
#include "Spring.h"

class MassSpring : public ISimulable
{
    uint32_t _index;

    std::vector<Node> _nodes;
    std::vector<Spring> _springs;

    neon::IdentifiableWrapper<PhysicsManager> _manager;
    neon::IdentifiableWrapper<neon::GameObject> _gameObject;

    void generateNodes(std::vector<rush::Vec3f>& positions, float mass, float dampingAlpha);

    void generateSprings(std::vector<uint32_t>& triangles, float dampingBeta, float stiffnessStretch,
                         float stiffnessBend);

  public:
    MassSpring(uint32_t dofIndexStart, neon::IdentifiableWrapper<PhysicsManager> manager,
               neon::IdentifiableWrapper<neon::GameObject> gameObject, std::vector<rush::Vec3f>& positions,
               std::vector<uint32_t>& triangles, float mass, float stiffnessStretch, float stiffnessBend,
               float dampingAlpha, float dampingBeta);

    [[nodiscard]] const std::vector<Node>& getNodes() const;

    [[nodiscard]] uint32_t getNumberOfDegreesOfFreedom() const override;

    void getPosition(Eigen::VectorXf& vector) const override;

    void setPosition(const Eigen::VectorXf& vector) override;

    void getVelocity(Eigen::VectorXf& vector) const override;

    void setVelocity(const Eigen::VectorXf& vector) override;

    void getForce(Eigen::VectorXf& vector) const override;

    void getForceJacobian(std::vector<Eigen::Triplet<float>>& dFdx,
                          std::vector<Eigen::Triplet<float>>& dFdv) const override;

    void getMass(std::vector<Eigen::Triplet<float>>& mass) const override;

    void getInverseMass(std::vector<Eigen::Triplet<float>>& mass) const override;

    void fixVector(Eigen::VectorXf& vector) const override;

    void fixMatrix(Eigen::SparseMatrix<float>& matrix) const override;
};

#endif //NEON_MASSSPRING_H
