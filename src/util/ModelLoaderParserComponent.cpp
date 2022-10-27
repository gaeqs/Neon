//
// Created by gaelr on 27/10/2022.
//

#include "ModelLoaderParserComponent.h"

#include <engine/GraphicComponent.h>

ModelLoaderParserComponent::ModelLoaderParserComponent(
        const std::string& shader,
        const ModelLoaderResult& result) :
        _shader(shader),
        _result(result) {
}

void ModelLoaderParserComponent::onStart() {
    auto gm = getGameObject();

    for (const auto& item: _result.models) {
        auto component = gm->newComponent<GraphicComponent>();
        component->setModel(item.first);
        if (item.second >= 0 && item.second < _result.materials.size()) {
            component->setMaterial(_result.materials[item.second]);
        }
        component->getMaterial().setShader(_shader);
    }

    destroy(); // The component is not needed anymore!
}
