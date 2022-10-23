//
// Created by grial on 19/10/22.
//

#ifndef RVTRACKING_SHADER_H
#define RVTRACKING_SHADER_H


#include <cstdint>
#include <string>
#include <memory>
#include <unordered_map>

#include <glm/glm.hpp>

#include <util/Resource.h>
#include <util/Result.h>

class Shader {

    uint32_t _id;
    std::unordered_map<std::string, int32_t> _uniformLocations;

    int32_t fetchUniformLocation(const std::string& name);

public:

    static Result<std::shared_ptr<Shader>, std::string> newShader(
            const Resource& vertex, const Resource& fragment);

    Shader(const Shader& other) = delete;

    explicit Shader(uint32_t id);

    ~Shader();

    void use() const;

    void setUniform(const std::string& name, bool value);

    void setUniform(const std::string& name, int value);

    void setUniform(const std::string& name, float value);

    void setUniform(const std::string& name, const glm::vec2& value);

    void setUniform(const std::string& name, const glm::vec3& value);

    void setUniform(const std::string& name, const glm::vec4& value);

    void setUniform(const std::string& name, const glm::mat4& value);

    void setUniform(const std::string& name, int32_t size, const float* array);
};


#endif //RVTRACKING_SHADER_H
