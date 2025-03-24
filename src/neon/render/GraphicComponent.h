//
// Created by grial on 19/10/22.
//

#ifndef RVTRACKING_GRAPHICCOMPONENT_H
#define RVTRACKING_GRAPHICCOMPONENT_H

#include <optional>
#include <memory>

#include <neon/render/shader/Material.h>
#include <neon/structure/Component.h>
#include <neon/render/model/Model.h>

namespace neon
{
    class GraphicComponent final : public Component
    {
        std::shared_ptr<Model> _model;
        std::optional<InstanceData::Instance> _modelTargetId;

        bool _firstPreDrawExecuted;

      public:
        GraphicComponent();

        explicit GraphicComponent(std::shared_ptr<Model> model);

        ~GraphicComponent() override;

        [[nodiscard]] const std::shared_ptr<Model>& getModel() const;

        void setModel(const std::shared_ptr<Model>& model);

        template<class InstanceData>
        void uploadData(size_t index, const InstanceData& data)
        {
            if (_modelTargetId.has_value()) {
                _model->getInstanceData(0)->uploadData(_modelTargetId.value(), index, data);
            }
        }

        void onPreDraw() override;

        void drawEditor() override;
    };

    REGISTER_COMPONENT(GraphicComponent, "Graphic Component")
} // namespace neon

#endif //RVTRACKING_GRAPHICCOMPONENT_H
