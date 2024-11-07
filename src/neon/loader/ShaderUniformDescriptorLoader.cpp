//
// Created by gaeqs on 1/11/24.
//

#include "ShaderUniformDescriptorLoader.h"


namespace neon {
    std::shared_ptr<ShaderUniformDescriptor>
    ShaderUniformDescriptorLoader::loadAsset(std::string name, nlohmann::json json, AssetLoaderContext context) {
        std::vector<nlohmann::json> rawBindings;

        if (auto& bindings = json["bindings"]; bindings.is_array()) {
            rawBindings = bindings;
        } else if (bindings.is_object()) {
            rawBindings.push_back(bindings);
        }

        std::vector<ShaderUniformBinding> bindings;

        for (auto& binding: rawBindings) {
            if (!binding.is_object()) continue;
            auto type = serialization::toUniformBindingType(binding.value("type", ""));
            if (!type.has_value()) continue;
            if (type == UniformBindingType::IMAGE) {
                bindings.push_back(ShaderUniformBinding::image());
            } else {
                if (!binding.contains("size")) continue;
                bindings.emplace_back(type.value(), binding["size"]);
            }
        }

        return std::make_shared<ShaderUniformDescriptor>(context.application, name, bindings);
    }
}
