//
// Created by gaeqs on 5/11/24.
//

#include "ModelLoader.h"

#include <neon/render/model/Mesh.h>

#include "AssetLoaderHelpers.h"

namespace neon {
    std::shared_ptr<Model>
    ModelLoader::loadAsset(std::string name, nlohmann::json json, AssetLoaderContext context) {
        ModelCreateInfo info;
        info.maximumInstances = json.value("maximum_instances", info.maximumInstances);
        info.uniformDescriptor = getAsset<ShaderUniformDescriptor>(json["uniform_descriptor"], context);
        info.shouldAutoFlush = json.value("auto_flush", info.shouldAutoFlush);

        auto& instanceTypes = json["custom_instance_sizes"];
        if (instanceTypes.is_array()) {
            std::vector<size_t> sizes;
            for (auto& entry: instanceTypes) {
                if (entry.is_number_unsigned()) {
                    sizes.push_back(entry.get<size_t>());
                }
            }

            info.instanceSizes = sizes;
            info.instanceTypes.clear();
            for (size_t _: info.instanceSizes) {
                info.instanceTypes.push_back(typeid(void));
            }
        }

        if (auto& meshes = json["meshes"]; meshes.is_array()) {
            for (auto& entry: meshes) {
                if (auto mesh = getAsset<Mesh>(entry, context); mesh != nullptr) {
                    info.drawables.push_back(mesh);
                }
            }
        }

        auto model = std::make_shared<Model>(context.application, name, info);
    }
}
