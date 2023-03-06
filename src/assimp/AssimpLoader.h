//
// Created by gaeqs on 14/02/2023.
//

#ifndef NEON_ASSIMPLOADER_H
#define NEON_ASSIMPLOADER_H

#include <functional>
#include <typeindex>
#include <utility>
#include <vector>

#include <glm/glm.hpp>

#include <cmrc/cmrc.hpp>

#include <engine/model/InputDescription.h>
#include <engine/model/DefaultInstancingData.h>
#include <engine/structure/IdentifiableWrapper.h>
#include <engine/shader/MaterialCreateInfo.h>


namespace neon {
    class Model;

    class Room;
}

struct aiScene;

namespace neon::assimp_loader {

    struct Result {

        /**
         * Whether the result is valid.
         */
        bool valid = false;

        /**
         * The loaded model.
         * This value is nullptr if the result is not valid.
         */
        std::shared_ptr<Model> model = nullptr;
    };

    struct VertexParserData {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 tangent;
        glm::vec4 color;
        glm::vec2 textureCoordinates;
    };

    struct VertexParser {

        using ParseFunction = std::function<void(const VertexParserData&,
                                                 std::vector<char>&)>;

        ParseFunction parseFunction;
        InputDescription description;
        size_t structSize;

        VertexParser(ParseFunction parseFunction_,
                     InputDescription description_,
                     size_t structSize_) :
                parseFunction(std::move(parseFunction_)),
                description(std::move(description_)),
                structSize(structSize_) {}

        template<typename Vertex>
        static VertexParser fromTemplate() {
            ParseFunction parseFunction = [](const VertexParserData& data,
                                             std::vector<char>& vec) {
                Vertex v = Vertex::fromAssimp(
                        data.position,
                        data.normal,
                        data.tangent,
                        data.color,
                        data.textureCoordinates
                );

                char* d = reinterpret_cast<char*>(&v);
                size_t size = sizeof(Vertex);
                for (size_t i = 0; i < size; ++i) {
                    vec.push_back(d[i]);
                }
            };

            return {parseFunction, Vertex::getDescription(), sizeof(Vertex)};
        }

    };

    struct InstanceData {
        InputDescription description;
        std::type_index type;
        size_t size;

        InstanceData(InputDescription description_,
                     const std::type_index& type_, size_t size_) :
                description(std::move(description_)), type(type_),
                size(size_) {}

        template<typename Instance>
        static InstanceData fromTemplate() {
            return InstanceData(
                    Instance::getInstancingDescription(),
                    typeid(Instance),
                    sizeof(Instance)
            );
        }
    };


    struct LoaderInfo {

        /**
         * The room where the model is loaded.
         */
        Room* room;

        /**
         * The name of the model.
         */
        std::string name;

        /**
         * The structure used to create materials.
         *
         * The vertex descriptor will be overridden
         * with the data inside vertexParser.
         *
         * The instance descriptor will be overridden
         * with the data inside instanceData.
         */
        MaterialCreateInfo materialCreateInfo;

        /**
         * The information about the vertex structure.
         * This data will be used to parse assimp vertices.
         *
         * This struct also contains the vertex descriptor to
         * be used by the loader.
         */
        VertexParser vertexParser;

        /**
         * The information about the instance structure
         * the loader will use.
         */
        InstanceData instanceData;

        /**
         * Whether UVs should be flipped.
         */
        bool flipUVs = true;

        /**
         * Whether winding order should be flipped.
         */
        bool flipWindingOrder = false;

        /**
         * Whether normals should be flipped.
         */
        bool flipNormals = false;

    private:

        LoaderInfo(Room* room_,
                   std::string name_,
                   MaterialCreateInfo materialCreateInfo_,
                   VertexParser vertexParser_,
                   InstanceData instanceData_) :
                room(room_),
                name(std::move(name_)),
                materialCreateInfo(std::move(materialCreateInfo_)),
                vertexParser(std::move(vertexParser_)),
                instanceData(std::move(instanceData_)) {}

    public:

        template<class Vertex, class Instance = DefaultInstancingData>
        static LoaderInfo create(Room* room,
                                 std::string name,
                                 MaterialCreateInfo materialCreateInfo) {
            return {
                    room,
                    std::move(name),
                    materialCreateInfo,
                    VertexParser::fromTemplate<Vertex>(),
                    InstanceData::fromTemplate<Instance>()
            };
        }

    };

    /**
     * Loads the model codified on the given CMRC file.
     *
     * @param file the file where the model is codified on.
     * @param info the information about the loading process.
     * @return the model.
     */
    Result load(const cmrc::file& file,
                const LoaderInfo& info);

    /**
     * Loads the model located at the given file.
     *
     * Always use direct file directions for models
     * codified in separated files!
     *
     * @param directory the directory where the file is located at.
     * @param file the file where the model is codified on.
     * @param info the information about the loading process.
     * @return the model.
     */
    Result load(const std::string& directory,
                const std::string& file,
                const LoaderInfo& info);

    /**
     * Loads the model codified in the given buffer.
     * @param buffer the buffer's pointer.
     * @param length the buffer's size.
     * @param info the information about the loading process.
     * @return the model.
     */
    Result load(const void* buffer,
                size_t length,
                const LoaderInfo& info);

    /**
     * Loads the model using an already loaded assimp scene.
     * @param scene the assimp scene.
     * @param info the information about the loading process.
     * @return the model.
     */
    Result load(const aiScene* scene,
                const LoaderInfo& info);
}


#endif //NEON_ASSIMPLOADER_H
