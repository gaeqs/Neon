//
// Created by gaelr on 03/11/2022.
//

#ifndef NEON_MODEL_H
#define NEON_MODEL_H

#include <string>

#include <neon/structure/Asset.h>
#include <neon/render/model/Drawable.h>
#include <neon/render/model/ModelCreateInfo.h>
#include <neon/render/model/InstanceData.h>

#ifdef USE_VULKAN

#include <vulkan/render/model/VKModel.h>

#endif

namespace neon {
    class Application;

    class Material;

    class CommandBuffer;

    /**
     * Represents a model that can be rendered
     * inside a scene.
     *
     * A model can be represented as a collection
     * of meshes that share the same position.
     */
    class Model : public Asset {
    public:
        static constexpr uint32_t DEFAULT_MAXIMUM_INSTANCES = 1024 * 16;

#ifdef USE_VULKAN
        using Implementation = vulkan::VKModel;
#endif

    private:
        std::vector<std::shared_ptr<Drawable>> _meshes;
        std::unique_ptr<ShaderUniformBuffer> _uniformBuffer;
        std::unordered_map<uint32_t, ModelBufferBinding> _bufferBindings;
        std::vector<std::unique_ptr<InstanceData>> _instanceDatas;
        bool _shouldAutoFlush;

        Implementation _implementation;

    public:
        Model(const Model& other) = delete;

        /**
         * Creates a model.
         * @param application the application holding the model.
         * @param name the name that identifies the model.
         * @param info the information about the model.
         */
        Model(Application* application,
              const std::string& name,
              const ModelCreateInfo& info);

        /**
         * Returns the implementation of the model.
         * @return the implementation.
         */
        [[nodiscard]] Implementation& getImplementation();

        /**
         * Returns the implementation of the model.
         * @return the implementation.
         */
        [[nodiscard]] const Implementation& getImplementation() const;

        /**
         * Returns the uniform buffer that contains the global data
         * of this model.
         * @return the uniform buffer.
         */
        [[nodiscard]] ShaderUniformBuffer* getUniformBuffer() const;

        [[nodiscard]] std::unordered_map<uint32_t, ModelBufferBinding>& getUniformBufferBindings();

        [[nodiscard]] const std::unordered_map<uint32_t, ModelBufferBinding>& getUniformBufferBindings() const;

        /**
        * Returns the list of instance datas present in this model.
        */
        [[nodiscard]] const std::vector<std::unique_ptr<InstanceData>>& getInstanceDatas() const;

        /**
        * Returns the instance data present at the given index.
        * This method return nullptr if the instance data doesn't exist.
        */
        [[nodiscard]] InstanceData* getInstanceData(size_t index) const;

        /**
         * Whether the renderer should call Model::flush() before rendering.
         * Set this flag to false if you want to manage the instance data
         * asyncronally.
         *
         * @return whether the renderer should call Model::flush() before
         * rendering.
         */
        [[nodiscard]] bool shouldAutoFlush() const;

        /**
        * Sets whetehr the renderer should call Model::flush() before rendering.
        * Set this flag to false if you want to manage the instance data
        * asyncronally.
        *
        * @param autoFlush whether the renderer should call Model::flush()
        * before rendering.
        */
        void setShouldAutoFlush(bool autoFlush);

        /**
         * Returns the list containing all meshes inside this model.
         * @return the meshes.
         */
        [[nodiscard]] const std::vector<std::shared_ptr<Drawable>>&
        getMeshes() const;

        /**
         * Return the amount of meshes this mode has.
         * @return the amount of meshes.
         */
        [[nodiscard]] size_t getMeshesAmount() const;

        /**
         * Returns the mesh located at the given index inside this model.
         * @param index the index.
         * @return the mesh.
         */
        [[nodiscard]] Drawable* getDrawable(uint32_t index) const;

        /**
         * Adds the given material to all meshes inside this model.
         * @param material the material.
         */
        void addMaterial(const std::shared_ptr<Material>& material) const;

        /**
         * Calls the draw calls for the meshes that
         * contains the given material.
         * <p>
         * This method can only be used
         * when a draw operation is being performed.
         *
         * @param material the material.
         */
        void draw(const Material* material) const;

        /**
         * Calls the draw calls for the meshes that
         * contains the given material.
         * <p>
         * This method can only be used
         * when a draw operation is being performed,
         * but doesn't require the material to be attached
         * to the model.
         * <p>
         * This method begins and ends a new render pass.
         *
         * @param material the material.
         * @param commandBuffer the command buffer to use.
         */
        void drawOutside(const Material* material,
                         const CommandBuffer* commandBuffer) const;
    };
}

#endif //NEON_MODEL_H
