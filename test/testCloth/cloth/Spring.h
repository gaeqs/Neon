//
// Created by grial on 20/02/23.
//

#ifndef NEON_SPRING_H
#define NEON_SPRING_H

#include <cstdint>
#include <vector>

#include <Eigen/Sparse>
#include <Eigen/Sparse>
#include <Eigen/Dense>

#include "Node.h"

enum class SpringType {
    STRETCH,
    BEND
};

class Spring {

    uint32_t _nodeA;
    uint32_t _nodeB;
    uint32_t _nodeADoFIndex;
    uint32_t _nodeBDoFIndex;

    SpringType _type;

    float _stiffness;
    float _damping;
    float _length0;

    float _length;

    Eigen::Vector3f _direction;
    Eigen::Vector3f _velocityBA;

public:

    Spring(uint32_t a, uint32_t b, SpringType type,
           float stiffness, float damping,
           const std::vector<Node>& nodes);

    void updateState(const std::vector<Node>& nodes);


    void getForce(Eigen::VectorXf& vector) const;

    void getForceJacobian(
            std::vector<Eigen::Triplet<float>>& dFdx,
            std::vector<Eigen::Triplet<float>>& dFdv) const;

};


#endif //NEON_SPRING_H
