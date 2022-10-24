//
// Created by grial on 19/10/22.
//

#ifndef RVTRACKING_GRAPHICCOMPONENT_H
#define RVTRACKING_GRAPHICCOMPONENT_H


#include <engine/Material.h>
#include <engine/Component.h>
#include <engine/Model.h>

class GraphicComponent : public Component {

    Material _material;
    IdentifiableWrapper<Model> _model;

public:

    GraphicComponent();

    const Material& getMaterial() const;

    Material& getMaterial();

    void setMaterial(const Material& material);

    const IdentifiableWrapper<Model>& getModel() const;

    IdentifiableWrapper<Model>& getModel();

    void setModel(const IdentifiableWrapper<Model>& model);

};


#endif //RVTRACKING_GRAPHICCOMPONENT_H
