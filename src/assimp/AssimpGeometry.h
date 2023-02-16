//
// Created by gaelr on 26/01/2023.
//

#ifndef NEON_ASSIMPGEOMETRY_H
#define NEON_ASSIMPGEOMETRY_H


#include <cstdint>
#include <unordered_map>

#include <assimp/scene.h>
#include <glm/glm.hpp>

namespace assimp_geometry {

    struct VertexFace {
        uint32_t vertex;
        uint32_t face;

        bool operator==(const VertexFace& other) const {
            return vertex == other.vertex && face == other.face;
        }
    };

    struct VertexFaceHash {
        size_t operator()(const VertexFace& e) const {
            return e.face + e.vertex;
        }
    };

    float cotan(const aiVector3D& a, const aiVector3D& b, const aiVector3D& c);

    bool isObtuse(const aiMesh* mesh, uint32_t face);

    std::vector<uint32_t> getOtherVertices(const aiFace* face, uint32_t vertex);

    std::unordered_map<VertexFace, float, VertexFaceHash>
    getInfluenceAreas(const aiMesh* mesh);

    std::vector<glm::vec3> calculateTangents(const aiMesh* mesh);

}


#endif //NEON_ASSIMPGEOMETRY_H
