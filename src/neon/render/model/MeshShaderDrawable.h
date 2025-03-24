//
// Created by gaeqs on 02/10/2024.
//

#ifndef MESHSHADERDRAWABLE_H
#define MESHSHADERDRAWABLE_H

#include <functional>
#include <neon/render/model/Drawable.h>

#ifdef USE_VULKAN
    #include <vulkan/render/model/VKMeshShaderDrawable.h>
#endif

namespace neon
{
    class Model;

    class MeshShaderDrawable : public Drawable
    {
      public:
#ifdef USE_VULKAN
        using Implementation = vulkan::VKMeshShaderDrawable;
#endif

      private:
        Implementation _implementation;

      public:
        MeshShaderDrawable(Application* application, const std::string& name, std::shared_ptr<Material> material);

        MeshShaderDrawable(Application* application, const std::string& name,
                           const std::unordered_set<std::shared_ptr<Material>>& materials);

        ~MeshShaderDrawable() override = default;

        [[nodiscard]] Implementation& getImplementation() override;

        [[nodiscard]] const Implementation& getImplementation() const override;

        void setGroups(rush::Vec<3, uint32_t> groups);

        void setGroupsSupplier(const std::function<rush::Vec<3, uint32_t>(const Model&)>& supplier);
    };
} // namespace neon

#endif //MESHSHADERDRAWABLE_H
