//
// Created by gaelr on 27/10/2022.
//

#ifndef VRTRACKING_MODELLOADERPARSERCOMPONENT_H
#define VRTRACKING_MODELLOADERPARSERCOMPONENT_H

#include <engine/Component.h>
#include <assimp/ModelLoader.h>

class ModelLoaderParserComponent : public Component {

    std::string _shader;
    const ModelLoaderResult& _result;

public:

    ModelLoaderParserComponent(const std::string& shader,
                               const ModelLoaderResult& result);

    void onStart() override;

};


#endif //VRTRACKING_MODELLOADERPARSERCOMPONENT_H
