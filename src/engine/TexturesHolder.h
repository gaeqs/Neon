//
// Created by grial on 21/10/22.
//

#ifndef RVTRACKING_TEXTURESHOLDER_H
#define RVTRACKING_TEXTURESHOLDER_H

#include <unordered_map>
#include <string>
#include <memory>
#include <optional>

#include <gl/Texture.h>

class TexturesHolder {

    std::unordered_map<std::string, std::shared_ptr<Texture>> _textures;

public:

    TexturesHolder();

    std::weak_ptr<Texture> createTexture(
            const std::string& name, const char* data,
            int32_t width, int32_t height);

    bool hasTexture(const std::string& name);

    std::optional<std::weak_ptr<Texture>> getTexture(const std::string& name);

    bool removeTexture(const std::string& name);
};


#endif //RVTRACKING_TEXTURESHOLDER_H
