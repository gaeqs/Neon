//
// Created by grial on 20/02/23.
//

#ifndef NEON_MASSSPRING_H
#define NEON_MASSSPRING_H

#include <vector>

#include "ISimulable.h"
#include "Node.h"
#include "Spring.h"

class MassSpring : public ISimulable {

    std::vector<Node> _nodes;
    std::vector<Spring> _springs;

    float _index;

};


#endif //NEON_MASSSPRING_H
