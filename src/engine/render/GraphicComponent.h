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
    class GraphicComponent final : public Component {

        std::shared_ptr<Model> _model;
        std::optional<uint32_t*> _modelTargetId;

        bool _firstPreDrawExecuted;

    public:

        GraphicComponent();

        explicit GraphicComponent(std::shared_ptr<Model> model);

        ~GraphicComponent() final;

        [[nodiscard]] const std::shared_ptr<Model>& getModel() const;

        void setModel(const std::shared_ptr<Model>& model);

        template<class InstanceData>
        void uploadData(size_t index, const InstanceData& data) {
            if (_modelTargetId.has_value()) {
                _model->uploadData(*_modelTargetId.value(), index, data);
            }
        }

        void onPreDraw() final;

        void drawEditor() final;

    };
    REGISTER_COMPONENT(GraphicComponent, "Graphic Component")
}

#endif //RVTRACKING_GRAPHICCOMPONENT_H
