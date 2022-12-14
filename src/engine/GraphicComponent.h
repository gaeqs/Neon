//
// Created by grial on 19/10/22.
//

#ifndef RVTRACKING_GRAPHICCOMPONENT_H
#define RVTRACKING_GRAPHICCOMPONENT_H

#include <optional>

#include <engine/IdentifiableWrapper.h>
#include <engine/Material.h>
#include <engine/Component.h>
#include <engine/model/Model.h>

class GraphicComponent : public Component {

    IdentifiableWrapper<Model> _model;
    std::optional<uint32_t*> _modelTargetId;

public:

    GraphicComponent();

    explicit GraphicComponent(IdentifiableWrapper<Model> model);

    ~GraphicComponent() override;

    [[nodiscard]] const IdentifiableWrapper<Model>& getModel() const;

    void setModel(const IdentifiableWrapper<Model>& model);

    template<class InstanceData>
    void uploadData(const InstanceData& data) {
        if (_modelTargetId.has_value()) {
            _model->uploadData(*_modelTargetId.value(), data);
        }
    }

    void onUpdate(float deltaTime) override;
};


#endif //RVTRACKING_GRAPHICCOMPONENT_H
