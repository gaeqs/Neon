//
// Created by grial on 20/02/23.
//

#ifndef NEON_ISIMULABLE_H
#define NEON_ISIMULABLE_H

#include <cstdint>

#include <Eigen/Sparse>
#include <Eigen/Sparse>
#include <Eigen/Dense>

class ISimulable {

public:

    virtual void initialize(int startIndex);

    virtual uint32_t getNumberOfDegreesOfFreedom() const = 0;

    virtual void getPosition(Eigen::VectorXf& vector) const = 0;

    virtual void setPosition(Eigen::VectorXf& vector);

    virtual void getVelocity(Eigen::VectorXf& vector) const = 0;

    virtual void setVelocity(Eigen::VectorXf& vector);

    virtual void getForce(Eigen::VectorXf& vector) const = 0;

    virtual void getForceJacobian(
            std::vector<Eigen::Triplet<float>>& dFdx,
            std::vector<Eigen::Triplet<float>>& dFdv) const = 0;

    virtual void getMass(std::vector<Eigen::Triplet<float>>& mass) const = 0;

    virtual void getInverseMass(
            std::vector<Eigen::Triplet<float>>& mass) const = 0;

    virtual void fixVector(Eigen::VectorXf& vector) const = 0;

    virtual void fixMatrix(Eigen::SparseMatrix<float>& matrix) const = 0;

};

#endif //NEON_ISIMULABLE_H
