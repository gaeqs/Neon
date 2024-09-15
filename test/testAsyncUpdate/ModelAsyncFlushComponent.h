//
// Created by gaeqs on 15/09/24.
//

#ifndef MODELASYNCFLUSHCOMPONENT_H
#define MODELASYNCFLUSHCOMPONENT_H

#include  <neon/Neon.h>

class ModelAsyncFlushComponent : public neon::Component {
    std::shared_ptr<neon::Model> _model;
    std::shared_ptr<neon::Task<void>> _task;

public:
    explicit ModelAsyncFlushComponent(std::shared_ptr<neon::Model> model);

    void onPreDraw() override;
};


#endif //MODELASYNCFLUSHCOMPONENT_H
