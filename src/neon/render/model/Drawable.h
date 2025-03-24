//
// Created by gaeqs on 29/09/2024.
//

#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <memory>
#include <unordered_set>
#include <neon/render/shader/Material.h>

#ifdef USE_VULKAN

    #include <vulkan/render/model/VKDrawable.h>

#endif

namespace neon
{
    class Drawable : public Asset
    {
        std::unordered_set<std::shared_ptr<Material>> _materials;

      public:
#ifdef USE_VULKAN
        using Implementation = vulkan::VKDrawable;
#endif

        Drawable(std::type_index type, std::string name);

        ~Drawable() override = default;

        [[nodiscard]] virtual Implementation& getImplementation() = 0;

        [[nodiscard]] virtual const Implementation& getImplementation() const = 0;

        /**
         * Returns the materials of the mesh.
         * @return the materials.
         */
        [[nodiscard]] const std::unordered_set<std::shared_ptr<Material>>& getMaterials() const;

        /**
         * Returns the materials of the mesh.
         * You can modify this vector to add new materials.
         * @return the materials.
         */
        [[nodiscard]] std::unordered_set<std::shared_ptr<Material>>& getMaterials();

        /**
         * Sets the material of the mesh.
         * This method overrides all materials inside
         * this mesh.
         * @param material the material.
         */
        void setMaterial(const std::shared_ptr<Material>& material);
    };
} // namespace neon

#endif //DRAWABLE_H
