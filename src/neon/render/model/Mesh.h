//
// Created by gaelr on 03/11/2022.
//

#ifndef NEON_MESH_H
#define NEON_MESH_H


#include <unordered_set>
#include <string>

#include <neon/render/shader/Material.h>
#include <neon/render/model/Drawable.h>

#ifdef USE_VULKAN

#include <vulkan/render/model/VKMesh.h>

#endif

namespace neon {
    class Application;

    /**
     * Represents a set of vertices and indices
     * used by models to render an object.
     */
    class Mesh : public Drawable {
    public:
#ifdef USE_VULKAN
        using Implementation = vulkan::VKMesh;
#endif

    private:
        Implementation _implementation;

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
             const std::unordered_set<std::shared_ptr<Material>>& materials,
             bool modifiableVertices = false,
             bool modifiableIndices = false);

        /**
         * Returns the implementation of the mesh.
         * @return the implementation.
         */
        [[nodiscard]] Implementation& getImplementation() override;

        /**
         * Returns the implementation of the mesh.
         * @return the implementation.
         */
        [[nodiscard]] const Implementation& getImplementation() const override;

        /**
         * Creates new buffers and uploads all the given
         * vertex data to the GPU.
         * All the previous vertex data stored in this mesh
         * will be lost.
         *
         * You can give several vectors with different types
         * to this function. Each given vector will create
         * a different buffer that can be accessed individually.
         *
         * @tparam Types the types of the data to upload.
         * @param data the data.
         */
        template<typename... Types>
        void uploadVertices(const std::vector<Types>&... data) {
            _implementation.uploadVertices(data...);
        }

        /**
         * Creates new buffers and uploads all the given
         * indices to the GPU.
         * All the previous indices stored in this mesh
         * will be lost.
         * @param indices the indices.
         */
        void uploadIndices(const std::vector<uint32_t>& indices) {
            _implementation.uploadIndices(indices);
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
        [[nodiscard]] std::vector<Vertex>
        getVertices(size_t index,
                    CommandBuffer* cmd = nullptr) const {
            return _implementation.getVertices<Vertex>(index, cmd);
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
        bool setVertices(size_t index,
                         const std::vector<Vertex>& vertices,
                         CommandBuffer* cmd = nullptr) const {
            return _implementation.setVertices(index, vertices, cmd);
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
         * @param index the index of the buffer.
         * @param data the vertices in raw format.
         * @param length the length of the data buffer.
         * @return whether the operation was successful.
         */
        bool setVertices(size_t index,
                         const void* data,
                         size_t length,
                         CommandBuffer* cmd = nullptr) const;

        /**
         * Returns the indices of this mesh.
         * <p>
         * This method will retrieve the indices from the GPU.
         * Be aware of performance issues!
         *
         * @return the indices.
         */
        [[nodiscard]] std::vector<uint32_t>
        getIndices(CommandBuffer* cmd = nullptr) const;

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
        [[nodiscard]] bool
        setIndices(const std::vector<uint32_t>& indices,
                   CommandBuffer* cmd = nullptr) const;
    };
}


#endif //NEON_MESH_H
