//
// Created by gaelr on 03/11/2022.
//

#ifndef NEON_MESH_H
#define NEON_MESH_H


#include <vector>
#include <string>

#include <engine/shader/Material.h>
#include <engine/shader/ShaderUniformBuffer.h>
#include <engine/structure/Asset.h>

#ifdef USE_VULKAN

#include <vulkan/VKMesh.h>

#endif

namespace neon {

    class Application;

    /**
     * Represents a set of vertices and indices
     * used by models to render an object.
     */
    class Mesh : public Asset {

    public:
#ifdef USE_VULKAN
        using Implementation = vulkan::VKMesh;
#endif

    private:

        Implementation _implementation;
        std::vector<std::shared_ptr<Material>> _materials;

    public:

        Mesh(const Mesh& other) = delete;

        /**
         * Creates a new mesh.
         * @param application the application where the mesh belongs to.
         * @param material the material of the mesh.
         * @param modifiableVertices whether this mesh's vertices can be modified.
         * @param modifiableIndices whether this mesh's indices can be modified.
         */
        Mesh(Application* application,
             const std::string& name,
             std::shared_ptr<Material> material,
             bool modifiableVertices = false,
             bool modifiableIndices = false);

        /**
         * Creates a new mesh.
         * @param application the application where the mesh belongs to.
         * @param materials the materials of the mesh.
         * @param modifiableVertices whether this mesh's vertices can be modified.
         * @param modifiableIndices whether this mesh's indices can be modified.
         */
        Mesh(Application* application,
             const std::string& name,
             std::vector<std::shared_ptr<Material>> materials,
             bool modifiableVertices = false,
             bool modifiableIndices = false);

        /**
         * Returns the implementation of the mesh.
         * @return the implementation.
         */
        [[nodiscard]] Implementation& getImplementation();

        /**
         * Returns the implementation of the mesh.
         * @return the implementation.
         */
        [[nodiscard]] const Implementation& getImplementation() const;

        /**
         * Sets the vertices and indices of this mesh.
         * <p>
         * This will recreate the mesh's buffers.
         * This method doesn't require the modification flags
         * to work.
         * You can use this method even if this mesh is not modifiable.
         * <p>
         * Use this method to upload your mesh data.
         * @tparam Vertex the vertex type.
         * @param vertices the vertices to upload.
         * @param indices the indices to upload.
         */
        template<class Vertex>
        void setMeshData(const std::vector<Vertex>& vertices,
                         const std::vector<uint32_t>& indices) {
            _implementation.uploadData(vertices, indices);
        }

        /**
         * Returns the vertices of this mesh.
         * <p>
         * This method will retrieve the vertices from the GPU.
         * Be aware of performance issues!
         *
         * @tparam Vertex the vertices.
         * @return the vertices.
         */
        template<class Vertex>
        [[nodiscard]] std::vector<Vertex> getVertices() const {
            return _implementation.getVertices<Vertex>();
        }

        /**
         * Modifies the vertices of this mesh.
         * <p>
         * The size of the vertices' buffer won't change.
         * If the given vector is bigger than the buffer,
         * the last vertices will be skipped.
         * If the given vector is smaller, the last vertices of the buffer
         * won't be modified.
         * <p>
         * This method requires vertices to be modifiable.
         *
         * @tparam Vertex the vertex type.
         * @param vertices the vertices.
         * @return whether the operation was successful.
         */
        template<class Vertex>
        bool setVertices(const std::vector<Vertex>& vertices) const {
            return _implementation.setVertices(vertices);
        }

        /**
         * Modifies the vertices of this mesh.
         * <p>
         * The size of the vertices' buffer won't change.
         * If the given vector is bigger than the buffer,
         * the last vertices of the vector will be skipped.
         * If the given vector is smaller, the last vertices of the buffer
         * won't be modified.
         * <p>
         * This method requires vertices to be modifiable.
         *
         * @param data the vertices in raw format.
         * @param length the length of the data buffer.
         * @return whether the operation was successful.
         */
        bool setVertices(const void* data, size_t length) const;

        /**
         * Returns the indices of this mesh.
         * <p>
         * This method will retrieve the indices from the GPU.
         * Be aware of performance issues!
         *
         * @return the indices.
         */
        [[nodiscard]] std::vector<uint32_t> getIndices() const;

        /**
         * Modifies the vertices of this mesh.
         * <p>
         * The size of the indices' buffer won't change.
         * If the given vector is bigger than the buffer,
         * the last indices of the vector will be skipped.
         * If the given vector is smaller, the last indices of the buffer
         * won't be modified.
         * <p>
         * This method requires indices to be modifiable.
         *
         * @param indices the indices.
         * @return whether the operation was successful.
         */
        bool setIndices(const std::vector<uint32_t>& indices) const;

        /**
         * Returns the materials of the mesh.
         * @return the materials.
         */
        [[nodiscard]] const std::vector<std::shared_ptr<Material>>&
        getMaterials() const;

        /**
         * Returns the materials of the mesh.
         * You can modify this vector to add new materials.
         * @return the materials.
         */
        [[nodiscard]] std::vector<std::shared_ptr<Material>>& getMaterials();

        /**
         * Sets the material of the mesh.
         * This method overrides all materials inside
         * this mesh.
         * @param material the material.
         */
        void setMaterial(const std::shared_ptr<Material>& material);

    };
}


#endif //NEON_MESH_H
