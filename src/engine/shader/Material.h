//
// Created by grial on 19/10/22.
//

#ifndef RVTRACKING_MATERIAL_H
#define RVTRACKING_MATERIAL_H

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include <engine/structure/Asset.h>
#include <engine/structure/IdentifiableWrapper.h>
#include <engine/shader/ShaderUniformBuffer.h>
#include <engine/shader/MaterialCreateInfo.h>
#include <engine/model/InputDescription.h>
#include <engine/render/Texture.h>
#include <engine/render/FrameBuffer.h>

#include <assimp/types.h>

#ifdef USE_VULKAN

#include <vulkan/shader/VKMaterial.h>

#endif

namespace neon {
    class ShaderProgram;

    class ShaderUniformDescriptor;

    class Application;

    class Material : public Asset {

    public:
#ifdef USE_VULKAN
        using Implementation = vulkan::VKMaterial;
#endif
    private:

        std::shared_ptr<ShaderProgram> _shader;

        std::shared_ptr<ShaderUniformDescriptor> _uniformDescriptor;
        ShaderUniformBuffer _uniformBuffer;

        Implementation _implementation;

    public:

        Material(const Material& other) = delete;

        Material(Application* application,
                 const std::string& name,
                 const MaterialCreateInfo& createInfo);

        [[nodiscard]] const std::shared_ptr<ShaderProgram>&
        getShader() const;

        [[nodiscard]] const ShaderUniformBuffer& getUniformBuffer() const;

        [[nodiscard]] ShaderUniformBuffer& getUniformBuffer();

        [[nodiscard]] const std::shared_ptr<ShaderUniformDescriptor>&
        getUniformDescriptor() const;

        [[nodiscard]] const Implementation& getImplementation() const;

        Implementation& getImplementation();

        void
        pushConstant(const std::string& name, const void* data, uint32_t size);

        template<class T>
        void pushConstant(const std::string key, const T& value) {
            pushConstant(key, &value, sizeof(T));
        }

        void setTexture(const std::string& name,
                        IdentifiableWrapper<Texture> texture);
    };
}


#endif //RVTRACKING_MATERIAL_H
