//
// Created by grial on 19/10/22.
//

#ifndef RVTRACKING_GRAPHICCOMPONENT_H
#define RVTRACKING_GRAPHICCOMPONENT_H


#include <engine/Material.h>
#include <engine/Component.h>

class GraphicComponent : public Component {

    Material _material;

public:

    GraphicComponent();

};


#endif //RVTRACKING_GRAPHICCOMPONENT_H
