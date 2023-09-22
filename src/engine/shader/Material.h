//
// Created by grial on 19/10/22.
//

#ifndef RVTRACKING_MATERIAL_H
#define RVTRACKING_MATERIAL_H

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include <engine/structure/Asset.h>
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

    /**
     * Represents a render material.
     * A material tells the engine how to render
     * a model.
     * Materials are primarily defined by a shader
     * program and a target frame buffer.
     * The model will be render at the defined framed buffer,
     * using the defined shader.
     * <p>
     * Materials can also contains images and uniform buffers that
     * the shader can use.
     * Unlike the shader and the target, the uniform buffers and
     * images are mutable and can be changed anytime.
     */
    class Material : public Asset {

    public:
#ifdef USE_VULKAN
        using Implementation = vulkan::VKMaterial;
#endif
    private:

        std::shared_ptr<ShaderProgram> _shader;
        std::shared_ptr<FrameBuffer> _target;
        std::unique_ptr<ShaderUniformBuffer> _uniformBuffer;

        int32_t _priority;

        Implementation _implementation;

    public:

        Material(const Material& other) = delete;

        /**
         * Creates a new material.
         * @param application the application holding this asset.
         * @param name the name of this material.
         * @param createInfo the creation information of this material.
         */
        Material(Application* application,
                 const std::string& name,
                 const MaterialCreateInfo& createInfo);

        /**
         * Returns the shader program this material uses.
         * This value cannot be modified.
         * @return the shader program.
         */
        [[nodiscard]] const std::shared_ptr<ShaderProgram>&
        getShader() const;

        /**
         * Returns the target frame buffer this material
         * will draw the meshes using it.
         * @return the target.
         */
        [[nodiscard]] const std::shared_ptr<FrameBuffer>&
        getTarget() const;

        /**
         * Returns the uniform buffer that contains the mutable information
         * of this material.
         * The uniform buffer may not be present if
         * no description has been provided.
         * @return the uniform buffer.
         */
        [[nodiscard]] const std::unique_ptr<ShaderUniformBuffer>&
        getUniformBuffer() const;

        /**
         * Returns the uniform buffer that contains the mutable information
         * of this material.
         * The uniform buffer may not be present if
         * no description has been provided.
         * @return the uniform buffer.
         */
        [[nodiscard]] std::unique_ptr<ShaderUniformBuffer>&
        getUniformBuffer();

        /**
         * Returns the priority of the material.
         * The bigger this number, the earlier this
         * material will be rendered inside its
         * target.
         * <p>
         * The default priority is 0. The priority
         * can be negative.
         * @return the priority.
         */
        [[nodiscard]] int32_t getPriority() const;

        /**
         * Sets the priority of the material.
         * The bigger this number, the earlier this
         * material will be rendered inside its
         * target.
         * <p>
         * The default priority is 0. The priority
         * can be negative.
         * @param priority the priority.
         */
        void setPriority(int32_t priority);

        /**
         * Returns the implementation of this material.
         * @return the implementation.
         */
        [[nodiscard]] const Implementation& getImplementation() const;

        /**
         * Returns the implementation of this material.
         * @return the implementation.
         */
        Implementation& getImplementation();

        /**
         * Sets the value of a shader constant.
         * @param name the name of the constant.
         * @param data the data to set.
         * @param size the size of the data array.
         */
        void pushConstant(const std::string& name,
                          const void* data, uint32_t size);

        /**
         * Sets the value of a shader constant.
         * @tparam T the type of the value.
         * @param key the name of the constant.
         * @param value the data to set.
         */
        template<class T>
        void pushConstant(const std::string key, const T& value) {
            pushConstant(key, &value, sizeof(T));
        }

        /**
         * Sets the texture of a shader sampler.
         * @param name the name of the sampler.
         * @param texture the texture.
         */
        void setTexture(const std::string& name,
                        std::shared_ptr<Texture> texture);

        // region Util static methods

        static std::unique_ptr<Material> create(
                Application* application,
                const std::string& name,
                const std::shared_ptr<FrameBuffer>& target,
                const std::shared_ptr<ShaderProgram>& shader,
                const InputDescription& vertex,
                const InputDescription& instance,
                const std::vector<std::pair<void*, size_t>>& buffers,
                const std::vector<std::shared_ptr<Texture>>& textures);

        // endregion
    };
}


#endif //RVTRACKING_MATERIAL_H
