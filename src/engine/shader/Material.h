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

#include <engine/Identifiable.h>
#include <engine/IdentifiableWrapper.h>
#include <engine/shader/ShaderUniformBuffer.h>
#include <engine/model/InputDescription.h>

#ifdef USE_VULKAN

#include <vulkan/shader/VKMaterial.h>
#include <assimp/types.h>

#endif

class ShaderProgram;

class ShaderUniformDescriptor;

class Room;

class Material : public Identifiable {

    template<class T> friend
    class IdentifiableWrapper;

public:
#ifdef USE_VULKAN
    using Implementation = VKMaterial;
#endif
private:

    uint64_t _id;
    IdentifiableWrapper<ShaderProgram> _shader;

    std::shared_ptr<ShaderUniformDescriptor> _uniformDescriptor;
    ShaderUniformBuffer _uniformBuffer;

    Implementation _implementation;

public:

    Material(const Material& other) = delete;

    Material(Room* room,
             IdentifiableWrapper<ShaderProgram> shader,
             const std::shared_ptr<ShaderUniformDescriptor>& descriptor,
             const InputDescription& vertexDescription,
             const InputDescription& instanceDescription);

    [[nodiscard]] uint64_t getId() const override;

    [[nodiscard]] const IdentifiableWrapper<ShaderProgram>& getShader() const;

    [[nodiscard]] const ShaderUniformBuffer& getUniformBuffer() const;

    [[nodiscard]] ShaderUniformBuffer& getUniformBuffer();

    [[nodiscard]] const std::shared_ptr<ShaderUniformDescriptor>&
    getUniformDescriptor() const;

    [[nodiscard]] const Implementation& getImplementation() const;

    Implementation& getImplementation();

    template<class T>
    void setValue(const std::string key, const T& value) {
    }
};


#endif //RVTRACKING_MATERIAL_H
