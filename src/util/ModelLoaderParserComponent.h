//
// Created by gaelr on 27/10/2022.
//

#ifndef VRTRACKING_MODELLOADERPARSERCOMPONENT_H
#define VRTRACKING_MODELLOADERPARSERCOMPONENT_H

#include <memory>

#include <engine/Component.h>
#include <assimp/ModelLoader.h>

class ModelLoaderParserComponent : public Component {

    std::string _shader;
    ModelLoaderResult _result;

public:

    ModelLoaderParserComponent(std::string  shader,
                               ModelLoaderResult  result);

    void onStart() override;

};


#endif //VRTRACKING_MODELLOADERPARSERCOMPONENT_H
