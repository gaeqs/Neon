//
// Created by gaeqs on 14/02/2023.
//

#ifndef NEON_ASSIMPLOADER_H
#define NEON_ASSIMPLOADER_H

#include <functional>
#include <typeindex>
#include <utility>
#include <vector>
#include <optional>

#include <rush/rush.h>

#include <cmrc/cmrc.hpp>

#include <engine/structure/collection/AssetCollection.h>
#include <engine/model/InputDescription.h>
#include <engine/model/DefaultInstancingData.h>
#include <engine/shader/MaterialCreateInfo.h>

namespace neon {
    class Model;

    class Room;
}

struct aiScene;

namespace neon::assimp_loader {

    struct VertexParserData {
        rush::Vec3f position;
        rush::Vec3f normal;
        rush::Vec3f tangent;
        rush::Vec4f color;
        rush::Vec2f textureCoordinates;
    };

    struct LocalMesh {
        std::vector<VertexParserData> vertices;
        std::vector<uint32_t> indices;
    };

    struct LocalModel {
        std::vector<LocalMesh> meshes;
    };

    enum class LoadError {
        INVALID_SCENE
    };

    struct Result {
        /**
         * Whether the result is valid.
         */
        std::optional<LoadError> error = {};

        /**
         * The loaded model.
         * This value is nullptr if the result is not valid.
         */
        std::shared_ptr<Model> model = nullptr;

        /**
         * The model data in CPU data.
         * This value is nullptr if the parameter
         * "loadLocalModel" has been set to false.
         */
        std::unique_ptr<LocalModel> localModel = nullptr;
    };

    struct VertexParser {
        using ParseFunction = std::function<void(const VertexParserData&,
                                                 std::vector<char>&)>;

        ParseFunction parseFunction;
        InputDescription description;
        size_t structSize;

        VertexParser(ParseFunction parseFunction_,
                     InputDescription description_,
                     size_t structSize_) : parseFunction(
                                               std::move(parseFunction_)),
                                           description(std::move(description_)),
                                           structSize(structSize_) {
        }

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
                     const std::type_index& type_,
                     size_t size_) : description(std::move(description_)),
                                     type(type_),
                                     size(size_) {
        }

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
         * The application where the model is loaded.
         */
        Application* application;

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
         * Whether materials should be loaded.
         */
        bool loadMaterials = true;

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

        /**
         * Whether the loaded models and meshes will be stored on the asset
         * collection of the application.
         *
         * If true, the assets can be retrieved from the asset collection
         * after the model and meshes have been loaded.
         */
        bool storeAssets = true;

        /**
         * The mode the meshes and models will be stored in the asset collection.
         */
        AssetStorageMode assetStorageMode = AssetStorageMode::WEAK;

        /**
          * Defines the command buffer used to create
          * the textures of the model.
          * If this command buffer is nullptr, the default
          * command buffer will be used.
          */
        CommandBuffer* commandBuffer = nullptr;

        /**
        * Whether the model data should be stored in CPU data too.
        */
        bool loadLocalModel = false;

    private:

        LoaderInfo(Application* application_,
                   std::string name_,
                   MaterialCreateInfo materialCreateInfo_,
                   VertexParser vertexParser_,
                   InstanceData instanceData_)
            : application(application_),
              name(std::move(name_)),
              materialCreateInfo(std::move(materialCreateInfo_)),
              vertexParser(std::move(vertexParser_)),
              instanceData(std::move(instanceData_)) {
        }

    public:

        template<class Vertex, class Instance = DefaultInstancingData>
        static LoaderInfo create(Application* application,
                                 std::string name,
                                 MaterialCreateInfo materialCreateInfo) {
            return {
                application,
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
