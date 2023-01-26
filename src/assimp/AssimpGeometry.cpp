//
// Created by gaelr on 26/01/2023.
//

#include <iostream>
#include "AssimpGeometry.h"

namespace assimp_geometry {

    bool isObtuse(aiMesh* mesh, uint32_t faceI) {
        auto face = mesh->mFaces[faceI];

        auto a = mesh->mVertices[face.mIndices[0]];
        auto b = mesh->mVertices[face.mIndices[1]];
        auto c = mesh->mVertices[face.mIndices[2]];

        if ((b - a) * (c - a) < 0) return true;
        if ((a - b) * (c - b) < 0) return true;
        if ((a - c) * (b - c) < 0) return true;
        return false;
    }

    float cotan(const aiVector3D& a, const aiVector3D& b, const aiVector3D& c) {
        auto ab = b - a;
        auto ac = c - a;
        float m = sqrt((ab * ab) * (ac * ac));
        float cosine = (ab * ac) / m;
        float sine = (ab ^ ac).Length() / m;
        return cosine / sine;
    }

    std::vector<uint32_t> getOtherVertices(aiFace* face, uint32_t vertex) {
        std::vector<uint32_t> vector;
        vector.reserve(face->mNumIndices - 1);
        for (int i = 0; i < face->mNumIndices; ++i) {
            auto current = face->mIndices[i];
            if (current == vertex) continue;
            vector.push_back(current);
        }
        return vector;
    }

    std::unordered_map<VertexFace, float, VertexFaceHash>
    getInfluenceAreas(aiMesh* mesh) {
        std::unordered_map<VertexFace, float, VertexFaceHash> map;
        for (uint32_t faceI = 0; faceI < mesh->mNumFaces; ++faceI) {
            auto face = mesh->mFaces[faceI];

            bool obtuse = isObtuse(mesh, faceI);

            for (uint32_t vertexI = 0; vertexI < face.mNumIndices; ++vertexI) {
                auto current = face.mIndices[vertexI];
                auto other = getOtherVertices(&face, current);

                auto a = mesh->mVertices[current];
                auto b = mesh->mVertices[other[0]];
                auto c = mesh->mVertices[other[1]];
                auto ab = b - a;
                auto ac = c - a;

                float area;
                if (obtuse) {
                    float triangleArea = (ab ^ ac).Length() * 0.5f;
                    float multiplier = (ab * ac) < 0 ? 0.5f : 0.25f;
                    area = triangleArea * multiplier;
                } else {
                    float bCot = cotan(b, a, c);
                    float cCot = cotan(c, a, b);
                    area = 0.125f * (ab.Length() * cCot +
                                    ac.Length() * bCot);
                }

                map[{current, faceI}] = area;
            }
        }


        return map;
    }

}