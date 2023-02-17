//
// Created by gaelr on 03/11/2022.
//

#ifndef NEON_MESH_H
#define NEON_MESH_H


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

        Mesh(Room* room, IdentifiableWrapper<Material> material,
             bool modifiableVertices = false,
             bool modifiableIndices = false);

        [[nodiscard]] uint64_t getId() const override;

        [[nodiscard]] Implementation& getImplementation();

        [[nodiscard]] const Implementation& getImplementation() const;

        template<class Vertex>
        void reinitializeVertexData(const std::vector<Vertex>& vertices,
                                    const std::vector<uint32_t>& indices) {
            _implementation.uploadData(vertices, indices);
        }

        template<class Vertex>
        std::vector<Vertex> getVertices() {
            return _implementation.getVertices<Vertex>();
        }

        [[nodiscard]] IdentifiableWrapper<Material> getMaterial() const;

        void setMaterial(const IdentifiableWrapper<Material>& material);

    };
}


#endif //NEON_MESH_H
