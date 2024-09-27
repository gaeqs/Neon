//
// Created by gaelr on 28/01/2023.
//

#ifndef NEON_MODELUTILS_H
#define NEON_MODELUTILS_H

#include <functional>
#include <memory>

#include <rush/rush.h>

#include <neon/Neon.h>

namespace neon::model_utils {
    template<class Vertex>
    std::shared_ptr<Model> createModel(
        Room* room,
        const std::string& name,
        std::shared_ptr<Material> material,
        const std::vector<Vertex>& vertices,
        ModelCreateInfo info = {}) {
        auto mesh = std::make_shared<Mesh>(
            room->getApplication(),
            "mesh", material
        );

        std::vector<uint32_t> indices;
        indices.reserve(vertices.size());
        for (uint32_t i = 0; i < vertices.size(); ++i) {
            indices.push_back(i);
        }

        mesh->uploadVertices(vertices);
        mesh->uploadIndices(indices);

        info.meshes.push_back(std::move(mesh));

        auto model = std::make_shared<Model>(
            room->getApplication(),
            name,
            info
        );

        return model;
    }

    template<class Vertex>
    std::shared_ptr<Model> createCubeModel(
        Room* room,
        std::shared_ptr<Material> material) {
        static const std::vector<uint32_t> CUBE_TRIANGLE_INDEX = {
            0, 1, 2, 1, 3, 2,
            4, 6, 5, 5, 6, 7,
            8, 10, 9, 9, 10, 11,
            12, 13, 14, 13, 15, 14,
            16, 17, 18, 17, 19, 18,
            20, 22, 21, 21, 22, 23,
        };

        static const rush::Vec3f CUBE_VERTICES[] = {
            rush::Vec3f(-1.0f, -1.0f, 1.0f), //0
            rush::Vec3f(1.0f, -1.0f, 1.0f), //1
            rush::Vec3f(-1.0f, 1.0f, 1.0f), //2
            rush::Vec3f(1.0f, 1.0f, 1.0f), //3
            rush::Vec3f(-1.0f, -1.0f, -1.0f), //4
            rush::Vec3f(1.0f, -1.0f, -1.0f), //5
            rush::Vec3f(-1.0f, 1.0f, -1.0f), //6
            rush::Vec3f(1.0f, 1.0f, -1.0f), //7
            rush::Vec3f(1.0f, -1.0f, -1.0f), //8
            rush::Vec3f(1.0f, -1.0f, 1.0f), //9
            rush::Vec3f(1.0f, 1.0f, -1.0f), //10
            rush::Vec3f(1.0f, 1.0f, 1.0f), //11
            rush::Vec3f(-1.0f, -1.0f, -1.0f), //12
            rush::Vec3f(-1.0f, -1.0f, 1.0f), //13
            rush::Vec3f(-1.0f, 1.0f, -1.0f), //14
            rush::Vec3f(-1.0f, 1.0f, 1.0f), //15
            rush::Vec3f(-1.0f, 1.0f, -1.0f), //16
            rush::Vec3f(-1.0f, 1.0f, 1.0f), //17
            rush::Vec3f(1.0f, 1.0f, -1.0f), //18
            rush::Vec3f(1.0f, 1.0f, 1.0f), //19
            rush::Vec3f(-1.0f, -1.0f, -1.0f), //20
            rush::Vec3f(-1.0f, -1.0f, 1.0f), //21
            rush::Vec3f(1.0f, -1.0f, -1.0f), //22
            rush::Vec3f(1.0f, -1.0f, 1.0f) //23
        };

        //Normal de los vertices
        const rush::Vec3f CUBE_VERTEX_NORMALS[] = {
            rush::Vec3f(0.0f, 0.0f, 1.0f),
            rush::Vec3f(0.0f, 0.0f, 1.0f),
            rush::Vec3f(0.0f, 0.0f, 1.0f),
            rush::Vec3f(0.0f, 0.0f, 1.0f),

            rush::Vec3f(0.0f, 0.0f, -1.0f),
            rush::Vec3f(0.0f, 0.0f, -1.0f),
            rush::Vec3f(0.0f, 0.0f, -1.0f),
            rush::Vec3f(0.0f, 0.0f, -1.0f),

            rush::Vec3f(1.0f, 0.0f, 0.0f),
            rush::Vec3f(1.0f, 0.0f, 0.0f),
            rush::Vec3f(1.0f, 0.0f, 0.0f),
            rush::Vec3f(1.0f, 0.0f, 0.0f),

            rush::Vec3f(-1.0f, 0.0f, 0.0f),
            rush::Vec3f(-1.0f, 0.0f, 0.0f),
            rush::Vec3f(-1.0f, 0.0f, 0.0f),
            rush::Vec3f(-1.0f, 0.0f, 0.0f),

            rush::Vec3f(0.0f, 1.0f, 0.0f),
            rush::Vec3f(0.0f, 1.0f, 0.0f),
            rush::Vec3f(0.0f, 1.0f, 0.0f),
            rush::Vec3f(0.0f, 1.0f, 0.0f),

            rush::Vec3f(0.0f, -1.0f, 0.0f),
            rush::Vec3f(0.0f, -1.0f, 0.0f),
            rush::Vec3f(0.0f, -1.0f, 0.0f),
            rush::Vec3f(0.0f, -1.0f, 0.0f)
        };

        const rush::Vec2f CUBE_VERTEX_TEX_COORDS[] = {
            rush::Vec2f(0.0f, 0.0f),
            rush::Vec2f(1.0f, 0.0f),
            rush::Vec2f(0.0f, 1.0f),
            rush::Vec2f(1.0f, 1.0f),

            rush::Vec2f(1.0f, 0.0f),
            rush::Vec2f(0.0f, 0.0f),
            rush::Vec2f(1.0f, 1.0f),
            rush::Vec2f(0.0f, 1.0f),

            rush::Vec2f(1.0f, 0.0f),
            rush::Vec2f(0.0f, 0.0f),
            rush::Vec2f(1.0f, 1.0f),
            rush::Vec2f(0.0f, 1.0f),

            rush::Vec2f(0.0f, 0.0f),
            rush::Vec2f(1.0f, 0.0f),
            rush::Vec2f(0.0f, 1.0f),
            rush::Vec2f(1.0f, 1.0f),

            rush::Vec2f(0.0f, 1.0f),
            rush::Vec2f(0.0f, 0.0f),
            rush::Vec2f(1.0f, 1.0f),
            rush::Vec2f(1.0f, 0.0f),

            rush::Vec2f(0.0f, 0.0f),
            rush::Vec2f(0.0f, 1.0f),
            rush::Vec2f(1.0f, 0.0f),
            rush::Vec2f(1.0f, 1.0f),
        };


        const rush::Vec3f CUBE_VERTEX_TANGENTS[] = {
            rush::Vec3f(1.0f, 0.0f, 0.0f),
            rush::Vec3f(1.0f, 0.0f, 0.0f),
            rush::Vec3f(1.0f, 0.0f, 0.0f),
            rush::Vec3f(1.0f, 0.0f, 0.0f),

            rush::Vec3f(-1.0f, 0.0f, 0.0f),
            rush::Vec3f(-1.0f, 0.0f, 0.0f),
            rush::Vec3f(-1.0f, 0.0f, 0.0f),
            rush::Vec3f(-1.0f, 0.0f, 0.0f),

            rush::Vec3f(0.0f, 0.0f, -1.0f),
            rush::Vec3f(0.0f, 0.0f, -1.0f),
            rush::Vec3f(0.0f, 0.0f, -1.0f),
            rush::Vec3f(0.0f, 0.0f, -1.0f),

            rush::Vec3f(0.0f, 0.0f, 1.0f),
            rush::Vec3f(0.0f, 0.0f, 1.0f),
            rush::Vec3f(0.0f, 0.0f, 1.0f),
            rush::Vec3f(0.0f, 0.0f, 1.0f),

            rush::Vec3f(1.0f, 0.0f, 0.0f),
            rush::Vec3f(1.0f, 0.0f, 0.0f),
            rush::Vec3f(1.0f, 0.0f, 0.0f),
            rush::Vec3f(1.0f, 0.0f, 0.0f),

            rush::Vec3f(1.0f, 0.0f, 0.0f),
            rush::Vec3f(1.0f, 0.0f, 0.0f),
            rush::Vec3f(1.0f, 0.0f, 0.0f),
            rush::Vec3f(1.0f, 0.0f, 0.0f),
        };

        std::vector<Vertex> vertices;
        vertices.reserve(24);

        for (int i = 0; i < 24; ++i) {
            vertices.push_back(Vertex::fromAssimp(
                CUBE_VERTICES[i],
                CUBE_VERTEX_NORMALS[i],
                CUBE_VERTEX_TANGENTS[i],
                rush::Vec4f(0.0f, 0.0f, 0.0f, 0.0f),
                CUBE_VERTEX_TEX_COORDS[i]
            ));
        }

        auto mesh = std::make_shared<Mesh>(room->getApplication(),
                                           "box", material);
        mesh->uploadVertices(vertices);
        mesh->uploadIndices(CUBE_TRIANGLE_INDEX);

        ModelCreateInfo info;
        info.meshes.push_back(std::move(mesh));

        auto model = std::make_shared<Model>(
            room->getApplication(),
            "box",
            info
        );

        room->getApplication()->getAssets().store(model,
                                                  AssetStorageMode::WEAK);
        return model;
    }
}


#endif //NEON_MODELUTILS_H
