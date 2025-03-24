//
// Created by grial on 22/11/22.
//

#ifndef NEON_SHADERUNIFORMDESCRIPTOR_H
#define NEON_SHADERUNIFORMDESCRIPTOR_H

#include <cstdint>
#include <vector>
#include <memory>
#include <neon/render/shader/ShaderUniformBinding.h>

#ifdef USE_VULKAN

    #include <vulkan/render/shader/VKShaderUniformDescriptor.h>

#endif

namespace neon
{
    class Application;

    class ShaderUniformDescriptor : public Asset
    {
      public:
#ifdef USE_VULKAN
        using Implementation = vulkan::VKShaderUniformDescriptor;
#endif

      private:
        Implementation _implementation;

      public:
        ShaderUniformDescriptor(const ShaderUniformDescriptor& other) = delete;

        ShaderUniformDescriptor(Application* application, std::string name,
                                const std::vector<ShaderUniformBinding>& bindings);

        [[nodiscard]] const Implementation& getImplementation() const;

        [[nodiscard]] Implementation& getImplementation();

        [[nodiscard]] const std::vector<ShaderUniformBinding>& getBindings() const;

        // region Static helper constructors

        /**
         * Creates a new ShaderUniformDescriptor
         * that contains the given amount of images.
         * @param application the application.
         * @param amount the amount of images.
         * @return the new ShaderUniformDescriptor.
         */
        static std::unique_ptr<ShaderUniformDescriptor> ofImages(Application* application, std::string name,
                                                                 uint32_t amount);

        // endregion
    };
} // namespace neon

#endif //NEON_SHADERUNIFORMDESCRIPTOR_H
