//
// Created by gaelr on 03/11/2022.
//

#ifndef NEON_MODEL_H
#define NEON_MODEL_H

#include <string>
#include <typeindex>

#include <engine/structure/Asset.h>
#include <engine/model/Mesh.h>

#ifdef USE_VULKAN

#include <vulkan/VKModel.h>

#endif

namespace neon {

    class Application;

    class Material;

    /**
     * Represents a model that can be rendered
     * inside a scene.
     *
     * A model can be represented as a collection
     * of meshes that share the same position.
     */
    class Model : public Asset {

    public:
#ifdef USE_VULKAN
        using Implementation = vulkan::VKModel;
#endif

    private:

        Implementation _implementation;
        std::vector<std::shared_ptr<Mesh>> _meshes;

        /**
         * Gets all implementations of the given meshes.
         * @param meshes the meshes.
         * @return the implementations.
         */
        static std::vector<Mesh::Implementation*> getMeshImplementations(
                const std::vector<std::shared_ptr<Mesh>>& meshes);

    public:

        Model(const Model& other) = delete;

        /**
         * Creates a model.
         * @param room the room where the model is located at.
         * @param meshes the meshes used by the model.
         */
        Model(Application* application,
              const std::string& name,
              std::vector<std::shared_ptr<Mesh>>& meshes);

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
        [[nodiscard]] const std::type_index& getInstancingStructType() const;

        /**
         * Defines the type of the structure used for instancing data.
         * Calling this method clears the instancing buffer.
         *
         * @tparam InstanceData the structure.
         */
        template<class InstanceData>
        void defineInstanceStruct() {
            _implementation.defineInstanceStruct<InstanceData>();
        }

        /**
         * Defines the type of the structure used for instancing data.
         * Calling this method clears the instancing buffer.
         *
         * @param type the type.
         * @param size the size of the struct.
         */
        void defineInstanceStruct(std::type_index type, size_t size);

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
         * Sets the instancing data of an instance.
         * @tparam InstanceData the type of the instance data.
         * @param id the identifier of the instance.
         * @param data the data to upload.
         */
        template<class InstanceData>
        void uploadData(uint32_t id, const InstanceData& data) {
            _implementation.uploadData(id, data);
        }

        /**
         * Sets the instancing data of an instance.
         * @param id the identifier of the instance.
         * @param raw the data to upload.
         */
        void uploadDataRaw(uint32_t id, const void* raw);

        /**
         * Uploads the instancing data to the GPU.
         *
         * This method is invoked automatically
         * before the room is rendered.
         */
        void flush();

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
        void draw(const std::shared_ptr<Material>& material) const;

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
         */
        void drawOutside(const std::shared_ptr<Material>& material) const;

    };
}

#endif //NEON_MODEL_H
