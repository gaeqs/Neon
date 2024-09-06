//
// Created by gaelr on 03/11/2022.
//

#ifndef NEON_MODEL_H
#define NEON_MODEL_H

#include <string>
#include <typeindex>

#include <neon/structure/Asset.h>
#include <neon/render/model/Mesh.h>
#include <neon/render/model/ModelCreateInfo.h>

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
        Implementation _implementation;
        std::vector<std::shared_ptr<Mesh>> _meshes;
        std::unique_ptr<ShaderUniformBuffer> _uniformBuffer;

        bool _shouldAutoFlush;

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
        [[nodiscard]] Model::Implementation& getImplementation();

        /**
         * Returns the implementation of the model.
         * @return the implementation.
         */
        [[nodiscard]] const Model::Implementation& getImplementation() const;

        /**
         * Returns the type of the structure used for instancing data,
         * @return the type.
         */
        [[nodiscard]] const std::vector<std::type_index>&
        getInstancingStructTypes() const;

        /**
         * Returns the uniform buffer that contains the global data
         * of this model.
         * @return the uniform buffer.
         */
        const std::unique_ptr<ShaderUniformBuffer>& getUniformBuffer() const;

        /**
         * Creates an instance of this model.
         * @return the identifier of the instance or the
         * error if something went wrong.
         */
        [[nodiscard]] Result<uint32_t*, std::string> createInstance();

        /**
         * Deletes an instance of this model.
         * @param id the idetifier of the model to delete.
         * @return whether the operation was successful.
         */
        bool freeInstance(uint32_t id);

        /**
         * Returns the amount of instances inside this model.
         * @return the amount of instances.
         */
        [[nodiscard]] size_t getInstanceAmount() const;

        /**
         * Sets the instancing data of an instance buffer.
         * @tparam InstanceData the type of the instance data.
         * @param id the identifier of the instance.
         * @param index the index of the instance buffer.
         * @param data the data to upload.
         */
        template<class InstanceData>
        void uploadData(uint32_t id, size_t index, const InstanceData& data) {
            _implementation.uploadData(id, index, data);
        }

        template<class InstanceData>
        const InstanceData* fetchData(uint32_t id, size_t index) const {
            return _implementation.fetchData<InstanceData>(id, index);
        }

        /**
         * Sets the instancing data of an instance buffer.
         * @param id the identifier of the instance.
         * @param index the index of the buffer,
         * @param raw the data to upload.
         */
        void uploadDataRaw(uint32_t id, size_t index, const void* raw);

        /**
         * Uploads the instancing data to the GPU.
         * <p>
         * This method is invoked automatically
         * before the room is rendered if shouldAutoFlush() is true.
         * <p>
         * You can provide this method an external command buffer.
         * This allows you to upload the model data asynchonously.
         */
        void flush(const CommandBuffer* commandBuffer = nullptr);

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
        [[nodiscard]] const std::vector<std::shared_ptr<Mesh>>&
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
        [[nodiscard]] Mesh* getMesh(uint32_t index);

        /**
         * Adds the given material to all meshes inside this model.
         * @param material the material.
         */
        void addMaterial(const std::shared_ptr<Material>& material);

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
