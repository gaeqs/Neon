//
// Created by grial on 22/11/22.
//

#include "ShaderUniformDescriptor.h"

uint64_t SHADER_UNIFORM_DESCRIPTOR_ID_GENERATOR = 1;

ShaderUniformDescriptor::ShaderUniformDescriptor(
        Application* application,
        const std::vector<ShaderUniformBinding>& bindings) :
        _id(SHADER_UNIFORM_DESCRIPTOR_ID_GENERATOR++),
        _implementation(application, bindings) {

}

const ShaderUniformDescriptor::Implementation&
ShaderUniformDescriptor::getImplementation() const {
    return _implementation;
}

ShaderUniformDescriptor::Implementation&
ShaderUniformDescriptor::getImplementation() {
    return _implementation;
}

const std::vector<ShaderUniformBinding>&
ShaderUniformDescriptor::getBindings() const {
    return _implementation.getBindings();
}
