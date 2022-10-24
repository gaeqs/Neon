//
// Created by gaelr on 23/10/2022.
//

#include "ShaderController.h"

#include <utility>

ShaderController::ShaderController(std::shared_ptr<Shader> shader) :
        _shader(std::move(shader)) {

}

const std::shared_ptr<Shader>& ShaderController::getShader() const {
    return _shader;
}
