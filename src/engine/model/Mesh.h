//
// Created by gaelr on 03/11/2022.
//

#ifndef NEON_MESH_H
#define NEON_MESH_H


#include <stdint.h>
#include <vector>

#include <engine/structure/Identifiable.h>
#include <engine/shader/Material.h>
#include <engine/shader/ShaderUniformBuffer.h>
#include <engine/collection/IdentifiableCollection.h>

#ifdef USE_VULKAN

#include <vulkan/VKMesh.h>

#endif

namespace neon {

    class Room;

    class Mesh : public Identifiable {

        template<class T> friend
        class IdentifiableWrapper;

    public:
#ifdef USE_VULKAN
        using Implementation = vulkan::VKMesh;
#endif

    private:

        uint64_t _id;
        Implementation _implementation;
        IdentifiableWrapper<Material> _material;

    public:

        Mesh(const Mesh& other) = delete;

        /**
         * Creates a new mesh.
         * @param room the room where the mesh belongs to.
         * @param material the material of the mesh.
         * @param modifiableVertices whether this mesh's vertices can be modified.
         * @param modifiableIndices whether this mesh's indices can be modified.
         */
        Mesh(Room* room, IdentifiableWrapper<Material> material,
             bool modifiableVertices = false,
             bool modifiableIndices = false);

        /**
         * Returns the id of the mesh.
         * This value is unique.
         * @return the id.
         */
        [[nodiscard]] uint64_t getId() const override;

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
         * Returns the material of the mesh.
         * @return the material.
         */
        [[nodiscard]] IdentifiableWrapper<Material> getMaterial() const;

        /**
         * Sets the material of the mesh.
         * @param material the material.
         */
        void setMaterial(const IdentifiableWrapper<Material>& material);

    };
}


#endif //NEON_MESH_H
