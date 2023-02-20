//
// Created by grial on 20/02/23.
//

#ifndef NEON_NODE_H
#define NEON_NODE_H

#include "ISimulable.h"

#include <cstdint>

#include <Eigen/Sparse>
#include <Eigen/Sparse>
#include <Eigen/Dense>

class Node : public ISimulable {

    uint32_t _doFIndex;
    float _mass;
    float _damping;
    bool _fixed;

    Eigen::Vector3f _position;
    Eigen::Vector3f _velocity;

public:

    [[nodiscard]] uint32_t getDoFIndex() const;

    [[nodiscard]] float getMassAsFloat() const;

    [[nodiscard]] bool isFixed() const;

    [[nodiscard]] const Eigen::Vector3f& getPositionVector() const;

    [[nodiscard]] const Eigen::Vector3f& getVelocityVector() const;

    void initialize(int startIndex) override;

    [[nodiscard]] uint32_t getNumberOfDegreesOfFreedom() const override;

    void getPosition(Eigen::VectorXf& vector) const override;

    void setPosition(Eigen::VectorXf& vector) override;

    void getVelocity(Eigen::VectorXf& vector) const override;

    void setVelocity(Eigen::VectorXf& vector) override;

    void getForce(Eigen::VectorXf& vector) const override;

    void getForceJacobian(
            std::vector<Eigen::Triplet<float>>& dFdx,
            std::vector<Eigen::Triplet<float>>& dFdv) const override;

    void getMass(std::vector<Eigen::Triplet<float>>& mass) const override;

    void getInverseMass(
            std::vector<Eigen::Triplet<float>>& mass) const override;

    void fixVector(Eigen::VectorXf& vector) const override;

    void fixMatrix(Eigen::SparseMatrix<float>& matrix) const override;

};


#endif //NEON_NODE_H
