//
// Created by gaelr on 22/12/2022.
//

#ifndef NEON_RENDERPASSSTRATEGY_H
#define NEON_RENDERPASSSTRATEGY_H

#include <functional>
#include <memory>
#include <neon/structure/Asset.h>

namespace neon
{
    class Room;

    class FrameBuffer;

    class Material;

    class Render;

    class RenderPassStrategy : public Asset
    {
        int _priority;

      public:
        explicit RenderPassStrategy(std::string name, int priority = 0);

        ~RenderPassStrategy() override = default;

        [[nodiscard]] int getPriority() const;

        virtual void render(Room* room, const Render* render,
                            const std::vector<std::shared_ptr<Material>>& sortedMaterials) const = 0;

        virtual bool requiresRecreation() = 0;

        virtual void recreate() = 0;
    };

    class DefaultRenderPassStrategy : public RenderPassStrategy
    {
        std::shared_ptr<FrameBuffer> _frameBuffer;

      public:
        DefaultRenderPassStrategy(std::string name, const std::shared_ptr<FrameBuffer>& frameBuffer, int priority = 0);

        [[nodiscard]] const std::shared_ptr<FrameBuffer>& getFrameBuffer() const;

        void render(Room* room, const Render* render,
                    const std::vector<std::shared_ptr<Material>>& sortedMaterials) const override;

        bool requiresRecreation() override;

        void recreate() override;
    };
} // namespace neon

#endif // NEON_RENDERPASSSTRATEGY_H
