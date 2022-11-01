//
// Created by grial on 19/10/22.
//

#ifndef RVTRACKING_MATERIAL_H
#define RVTRACKING_MATERIAL_H

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include <engine/IdentifiableWrapper.h>

class Shader;

class Texture;

class Room;

constexpr size_t MAX_ARRAY_SIZE = 16;

enum class MaterialEntryType {
    IMAGE,
    DATA
};

struct MaterialEntry {
    MaterialEntryType type;
    std::vector<char> data;
};

class Material {

    std::string _shader;
    std::unordered_map<std::string, MaterialEntry> _uniformValues;

public:

    const std::string& getShader() const;

    void setShader(const std::string& shader);

    const std::unordered_map<std::string, MaterialEntry>&
    getUniformValues() const;

    void setImage(const std::string& name, uint64_t imageId);

    void setImage(const std::string& name, IdentifiableWrapper<Texture> image);

    template<class T>
    void setValue(const std::string& name, const T& value) {
        std::vector<char> data;

        size_t size = sizeof(T);
        if ((size & 0b11) != 0) {
            size += 4 - (size & 0b11);
        }
        data.resize(size);


        memcpy(data.data(), &value, sizeof(T));
        _uniformValues[name] = {MaterialEntryType::DATA, data};
    }


};


#endif //RVTRACKING_MATERIAL_H
