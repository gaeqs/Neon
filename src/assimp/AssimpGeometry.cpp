//
// Created by gaelr on 26/01/2023.
//

#include <iostream>
#include "AssimpGeometry.h"

namespace assimp_geometry {

    bool isObtuse(const aiMesh* mesh, uint32_t faceI) {
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

    std::vector<uint32_t> getOtherVertices(const aiFace* face, uint32_t vertex) {
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
    getInfluenceAreas(const aiMesh* mesh) {
        std::unordered_map<VertexFace, float, VertexFaceHash> map;
        for (uint32_t faceI = 0; faceI < mesh->mNumFaces; ++faceI) {
            auto face = mesh->mFaces[faceI];

            bool obtuse = isObtuse(mesh, faceI);

            for (uint32_t vertexI = 0; vertexI < face.mNumIndices; ++vertexI) {
                auto current = face.mIndices[vertexI];
                auto other = getOtherVertices(&face, current);

                if(other.size() < 2) break;

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

    std::vector<glm::vec3> calculateTangents(const aiMesh* mesh) {
        std::vector<glm::vec3> tangents;
        std::vector<float> count;
        tangents.resize(mesh->mNumVertices);
        count.resize(mesh->mNumVertices);

        auto areas = assimp_geometry::getInfluenceAreas(mesh);

        for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
            auto face = mesh->mFaces[faceIndex];

            auto posA = mesh->mVertices[face.mIndices[0]];
            auto posB = mesh->mVertices[face.mIndices[1]];
            auto posC = mesh->mVertices[face.mIndices[2]];

            auto uvA = mesh->mTextureCoords[0][face.mIndices[0]];
            auto uvB = mesh->mTextureCoords[0][face.mIndices[1]];
            auto uvC = mesh->mTextureCoords[0][face.mIndices[2]];

            auto edge1 = posB - posA;
            auto edge2 = posC - posA;
            auto deltaUV1 = uvB - uvA;
            auto deltaUV2 = uvC - uvA;

            float f =
                    1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
            auto t = glm::normalize(glm::vec3{
                    f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
                    f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
                    f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z)
            });

            auto b = glm::normalize(glm::vec3{
                    f * (deltaUV2.x * edge2.x - deltaUV1.x * edge1.x),
                    f * (deltaUV2.x * edge2.y - deltaUV1.x * edge1.y),
                    f * (deltaUV2.x * edge2.z - deltaUV1.x * edge1.z)
            });


            for (uint32_t cvIndex = 0; cvIndex < face.mNumIndices; ++cvIndex) {
                auto currentVertex = face.mIndices[cvIndex];
                auto an = mesh->mNormals[currentVertex];
                auto n = glm::vec3(an.x, an.y, an.z);

                // Check handedness
                auto tv = t;
                if (glm::dot(glm::cross(n, t), b) < 0.0f) {
                    tv *= 1.0f;
                }
                auto area = areas[{currentVertex, faceIndex}];
                tangents[currentVertex] = tv * area;
                count[currentVertex] += area;
            }
        }

        // Calculate average.
        for (uint32_t i = 0; i < mesh->mNumVertices; ++i) {
            tangents[i] / static_cast<float>(count[i]);
        }

        // Orthogonalize
        for (uint32_t i = 0; i < mesh->mNumVertices; ++i) {
            auto t = tangents[i];
            auto an = mesh->mNormals[i];
            auto n = glm::vec3(an.x, an.y, an.z);
            t = glm::normalize(t - n * glm::dot(n, t));
            tangents[i] = t;
        }

        return tangents;
    }

}