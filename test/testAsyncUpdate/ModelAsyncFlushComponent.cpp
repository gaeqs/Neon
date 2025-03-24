//
// Created by gaeqs on 15/09/24.
//

#include "ModelAsyncFlushComponent.h"

ModelAsyncFlushComponent::ModelAsyncFlushComponent(std::shared_ptr<neon::Model> model) :
    _model(std::move(model))
{
}

void ModelAsyncFlushComponent::onPreDraw()
{
    _task = getTaskRunner().executeAsync([model = _model, task = _task] {
        if (task != nullptr) {
            task->wait();
        }
        model->getInstanceData(0)->flush();
    });
}
