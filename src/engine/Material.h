//
// Created by grial on 19/10/22.
//

#ifndef RVTRACKING_MATERIAL_H
#define RVTRACKING_MATERIAL_H

#include <string>
#include <glm/glm.hpp>
#include <unordered_map>

class Shader;

constexpr size_t MAX_ARRAY_SIZE = 16;

struct MaterialEntry {
    int32_t floatAmount;
    float floats[MAX_ARRAY_SIZE];
};

class Material {

    std::string _shader;
    std::unordered_map<std::string, MaterialEntry> _uniformValues;

public:

    const std::string& getShader() const;

    void setShader(const std::string& shader);

    template<class T>
    void setUniform(const std::string& name, const T& value) {
        MaterialEntry entry{};
        entry.floatAmount = std::min(MAX_ARRAY_SIZE, sizeof(T) / 4);
        memcpy(entry.floats, &value, std::min(MAX_ARRAY_SIZE * 4, sizeof(T)));
        _uniformValues[name] = entry;
    }

    void uploadUniforms(Shader& shader);


};


#endif //RVTRACKING_MATERIAL_H
