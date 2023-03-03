//
// Created by grial on 19/10/22.
//

#ifndef RVTRACKING_GRAPHICCOMPONENT_H
#define RVTRACKING_GRAPHICCOMPONENT_H

#include <optional>
#include <memory>

#include <engine/shader/Material.h>
#include <engine/structure/Component.h>
#include <engine/model/Model.h>

namespace neon {
    class GraphicComponent : public Component {

        std::shared_ptr<Model> _model;
        std::optional<uint32_t*> _modelTargetId;

    public:

        GraphicComponent();

        explicit GraphicComponent(std::shared_ptr<Model> model);

        ~GraphicComponent() override;

        [[nodiscard]] const std::shared_ptr<Model>& getModel() const;

        void setModel(const std::shared_ptr<Model>& model);

        template<class InstanceData>
        void uploadData(const InstanceData& data) {
            if (_modelTargetId.has_value()) {
                _model->uploadData(*_modelTargetId.value(), data);
            }
        }

        void onPreDraw() override;

        void drawEditor() override;

    };
    REGISTER_COMPONENT(GraphicComponent, "Graphic Component")
}

#endif //RVTRACKING_GRAPHICCOMPONENT_H
