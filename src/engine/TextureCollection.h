//
// Created by grial on 21/10/22.
//

#ifndef RVTRACKING_TEXTURECOLLECTION_H
#define RVTRACKING_TEXTURECOLLECTION_H

#include <unordered_map>
#include <string>
#include <memory>
#include <optional>

#include <gl/Texture.h>

class TextureCollection {

    std::unordered_map<std::string, std::shared_ptr<Texture>> _textures;

public:

    TextureCollection();

    std::weak_ptr<Texture> createTexture(
            const std::string& name, const char* data,
            int32_t width, int32_t height);

    bool hasTexture(const std::string& name);

    std::optional<std::weak_ptr<Texture>> getTexture(const std::string& name);

    bool removeTexture(const std::string& name);
};


#endif //RVTRACKING_TEXTURECOLLECTION_H
