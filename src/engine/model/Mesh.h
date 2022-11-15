//
// Created by gaelr on 03/11/2022.
//

#ifndef NEON_MESH_H
#define NEON_MESH_H


#include <vector>

#include <engine/Identifiable.h>
#include <engine/Material.h>
#include <engine/shader/ShaderUniformBuffer.h>
#include <engine/collection/IdentifiableCollection.h>

#ifdef USE_OPENGL

#include <gl/GLMesh.h>

#endif
#ifdef USE_VULKAN

#include <vulkan/VKMesh.h>

#endif

class Application;

class Mesh : public Identifiable {

    template<class T> friend
    class IdentifiableWrapper;

public:
#ifdef USE_OPENGL
    using Implementation = GLMesh;
#endif
#ifdef USE_VULKAN
    using Implementation = VKMesh;
#endif

private:

    uint64_t _id;
    Implementation _implementation;
    Material _material;

public:

    Mesh(Application* application);

    [[nodiscard]] uint64_t getId() const override;

    [[nodiscard]] Implementation& getImplementation();

    [[nodiscard]] const Implementation& getImplementation() const;

    template<class Vertex>
    void uploadVertexData(const std::vector<Vertex>& vertices,
                          const std::vector<uint32_t>& indices) {
        _implementation.uploadData(vertices, indices);
    }

    Material& getMaterial();

    [[nodiscard]] const Material& getMaterial() const;

    void setMaterial(const Material& material);

};


#endif //NEON_MESH_H
