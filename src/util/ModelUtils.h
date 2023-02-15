//
// Created by gaelr on 28/01/2023.
//

#ifndef NEON_MODELUTILS_H
#define NEON_MODELUTILS_H

#include <functional>

#include <glm/glm.hpp>

#include <engine/Engine.h>

namespace model_utils {

    template<class Vertex>
    IdentifiableWrapper<Model> createCubeModel(
            Room* room,
            const std::vector<IdentifiableWrapper<Texture>>& inputTextures,
            const std::shared_ptr<FrameBuffer>& target,
            IdentifiableWrapper<ShaderProgram> shader,
            const std::function<void(MaterialCreateInfo&)>& populateFunction) {

        static const std::vector<uint32_t> CUBE_TRIANGLE_INDEX = {
                0, 1, 2, 1, 3, 2,
                4, 6, 5, 5, 6, 7,
                8, 10, 9, 9, 10, 11,
                12, 13, 14, 13, 15, 14,
                16, 17, 18, 17, 19, 18,
                20, 22, 21, 21, 22, 23,
        };

        static const glm::vec3 CUBE_VERTICES[] = {
                glm::vec3(-1.0f, -1.0f, 1.0f), //0
                glm::vec3(1.0f, -1.0f, 1.0f), //1
                glm::vec3(-1.0f, 1.0f, 1.0f), //2
                glm::vec3(1.0f, 1.0f, 1.0f), //3
                glm::vec3(-1.0f, -1.0f, -1.0f), //4
                glm::vec3(1.0f, -1.0f, -1.0f), //5
                glm::vec3(-1.0f, 1.0f, -1.0f), //6
                glm::vec3(1.0f, 1.0f, -1.0f), //7
                glm::vec3(1.0f, -1.0f, -1.0f), //8
                glm::vec3(1.0f, -1.0f, 1.0f), //9
                glm::vec3(1.0f, 1.0f, -1.0f), //10
                glm::vec3(1.0f, 1.0f, 1.0f), //11
                glm::vec3(-1.0f, -1.0f, -1.0f), //12
                glm::vec3(-1.0f, -1.0f, 1.0f), //13
                glm::vec3(-1.0f, 1.0f, -1.0f), //14
                glm::vec3(-1.0f, 1.0f, 1.0f), //15
                glm::vec3(-1.0f, 1.0f, -1.0f), //16
                glm::vec3(-1.0f, 1.0f, 1.0f), //17
                glm::vec3(1.0f, 1.0f, -1.0f), //18
                glm::vec3(1.0f, 1.0f, 1.0f), //19
                glm::vec3(-1.0f, -1.0f, -1.0f), //20
                glm::vec3(-1.0f, -1.0f, 1.0f), //21
                glm::vec3(1.0f, -1.0f, -1.0f), //22
                glm::vec3(1.0f, -1.0f, 1.0f)  //23
        };

        //Normal de los vertices
        const glm::vec3 CUBE_VERTEX_NORMALS[] = {
                glm::vec3(0.0f, 0.0f, 1.0f),
                glm::vec3(0.0f, 0.0f, 1.0f),
                glm::vec3(0.0f, 0.0f, 1.0f),
                glm::vec3(0.0f, 0.0f, 1.0f),

                glm::vec3(0.0f, 0.0f, -1.0f),
                glm::vec3(0.0f, 0.0f, -1.0f),
                glm::vec3(0.0f, 0.0f, -1.0f),
                glm::vec3(0.0f, 0.0f, -1.0f),

                glm::vec3(1.0f, 0.0f, 0.0f),
                glm::vec3(1.0f, 0.0f, 0.0f),
                glm::vec3(1.0f, 0.0f, 0.0f),
                glm::vec3(1.0f, 0.0f, 0.0f),

                glm::vec3(-1.0f, 0.0f, 0.0f),
                glm::vec3(-1.0f, 0.0f, 0.0f),
                glm::vec3(-1.0f, 0.0f, 0.0f),
                glm::vec3(-1.0f, 0.0f, 0.0f),

                glm::vec3(0.0f, 1.0f, 0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f),

                glm::vec3(0.0f, -1.0f, 0.0f),
                glm::vec3(0.0f, -1.0f, 0.0f),
                glm::vec3(0.0f, -1.0f, 0.0f),
                glm::vec3(0.0f, -1.0f, 0.0f)
        };

        const glm::vec2 CUBE_VERTEX_TEX_COORDS[] = {
                glm::vec2(0.0f, 0.0f),
                glm::vec2(1.0f, 0.0f),
                glm::vec2(0.0f, 1.0f),
                glm::vec2(1.0f, 1.0f),

                glm::vec2(1.0f, 0.0f),
                glm::vec2(0.0f, 0.0f),
                glm::vec2(1.0f, 1.0f),
                glm::vec2(0.0f, 1.0f),

                glm::vec2(1.0f, 0.0f),
                glm::vec2(0.0f, 0.0f),
                glm::vec2(1.0f, 1.0f),
                glm::vec2(0.0f, 1.0f),

                glm::vec2(0.0f, 0.0f),
                glm::vec2(1.0f, 0.0f),
                glm::vec2(0.0f, 1.0f),
                glm::vec2(1.0f, 1.0f),

                glm::vec2(0.0f, 1.0f),
                glm::vec2(0.0f, 0.0f),
                glm::vec2(1.0f, 1.0f),
                glm::vec2(1.0f, 0.0f),

                glm::vec2(0.0f, 0.0f),
                glm::vec2(0.0f, 1.0f),
                glm::vec2(1.0f, 0.0f),
                glm::vec2(1.0f, 1.0f),
        };


        const glm::vec3 CUBE_VERTEX_TANGENTS[] = {
                glm::vec3(1.0f, 0.0f, 0.0f),
                glm::vec3(1.0f, 0.0f, 0.0f),
                glm::vec3(1.0f, 0.0f, 0.0f),
                glm::vec3(1.0f, 0.0f, 0.0f),

                glm::vec3(-1.0f, 0.0f, 0.0f),
                glm::vec3(-1.0f, 0.0f, 0.0f),
                glm::vec3(-1.0f, 0.0f, 0.0f),
                glm::vec3(-1.0f, 0.0f, 0.0f),

                glm::vec3(0.0f, 0.0f, -1.0f),
                glm::vec3(0.0f, 0.0f, -1.0f),
                glm::vec3(0.0f, 0.0f, -1.0f),
                glm::vec3(0.0f, 0.0f, -1.0f),

                glm::vec3(0.0f, 0.0f, 1.0f),
                glm::vec3(0.0f, 0.0f, 1.0f),
                glm::vec3(0.0f, 0.0f, 1.0f),
                glm::vec3(0.0f, 0.0f, 1.0f),

                glm::vec3(1.0f, 0.0f, 0.0f),
                glm::vec3(1.0f, 0.0f, 0.0f),
                glm::vec3(1.0f, 0.0f, 0.0f),
                glm::vec3(1.0f, 0.0f, 0.0f),

                glm::vec3(1.0f, 0.0f, 0.0f),
                glm::vec3(1.0f, 0.0f, 0.0f),
                glm::vec3(1.0f, 0.0f, 0.0f),
                glm::vec3(1.0f, 0.0f, 0.0f),
        };

        std::vector<Vertex> vertices;
        vertices.reserve(24);

        for (int i = 0; i < 24; ++i) {
            vertices.push_back(Vertex::fromAssimp(
                    CUBE_VERTICES[i],
                    CUBE_VERTEX_NORMALS[i],
                    CUBE_VERTEX_TANGENTS[i],
                    glm::vec4(0.0f, 0.0f, 0.0f, 0.0f),
                    CUBE_VERTEX_TEX_COORDS[i]
            ));
        }

        // Add an image binding for each given texture.
        std::vector<ShaderUniformBinding> materialBindings;
        materialBindings.resize(
                inputTextures.size(),
                ShaderUniformBinding{UniformBindingType::IMAGE, 0}
        );

        std::shared_ptr<ShaderUniformDescriptor> materialDescriptor;
        materialDescriptor = std::make_shared<ShaderUniformDescriptor>(
                room->getApplication(),
                materialBindings
        );

        MaterialCreateInfo info(target, shader);
        info.descriptions.uniform = materialDescriptor;
        info.descriptions.vertex = Vertex::getDescription();

        if (populateFunction != nullptr)
            populateFunction(info);

        auto material = room->getMaterials().create(info);

        for (uint32_t i = 0; i < inputTextures.size(); ++i) {
            material->getUniformBuffer().setTexture(i, inputTextures[i]);
        }

        auto mesh = std::make_unique<Mesh>(room, material);
        mesh->uploadVertexData(vertices, CUBE_TRIANGLE_INDEX);
        std::vector<std::unique_ptr<Mesh>> meshes;
        meshes.push_back(std::move(mesh));

        auto model = room->getModels().create(meshes);
        return model;
    }

}


#endif //NEON_MODELUTILS_H
