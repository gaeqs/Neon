//
// Created by grial on 19/10/22.
//

#ifndef RVTRACKING_RENDERER_H
#define RVTRACKING_RENDERER_H


#include <memory>

#include <util/ClusteredLinkedCollection.h>

class GraphicComponent;

class Renderer {

public:

    using ComponentList = ClusteredLinkedCollection<GraphicComponent>;

    Renderer(const Renderer& other) = delete;

    Renderer() = default;

    virtual ~Renderer() = default;

    virtual void render(std::shared_ptr<ComponentList> elements) = 0;
};


#endif //RVTRACKING_RENDERER_H
