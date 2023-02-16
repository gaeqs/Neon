//
// Created by grial on 22/11/22.
//

#ifndef NEON_SHADERUNIFORMDESCRIPTOR_H
#define NEON_SHADERUNIFORMDESCRIPTOR_H

#include <cstdint>
#include <vector>
#include <memory>
#include <engine/structure/Identifiable.h>
#include <engine/shader/ShaderUniformBinding.h>

#ifdef USE_VULKAN

#include "vulkan/shader/VKShaderUniformDescriptor.h"

#endif

class Application;

class ShaderUniformDescriptor : public Identifiable {

    template<class T> friend
    class IdentifiableWrapper;

public:
#ifdef USE_VULKAN
    using Implementation = VKShaderUniformDescriptor;
#endif

private:

    uint64_t _id;
    Implementation _implementation;

public:

    ShaderUniformDescriptor(const ShaderUniformDescriptor& other) = delete;

    ShaderUniformDescriptor(Application* application,
                            const std::vector<ShaderUniformBinding>& bindings);

    [[nodiscard]] uint64_t getId() const override;

    [[nodiscard]] const Implementation& getImplementation() const;

    [[nodiscard]] Implementation& getImplementation();

    [[nodiscard]] const std::vector<ShaderUniformBinding>& getBindings() const;


    // region Static helper constructors

    /**
     * Creates a new ShaderUniformDescriptor
     * that contains the given amount of images.
     * @param application the application.
     * @param amount the amount of images.
     * @return the new ShaderUniformDescriptor.
     */
    static std::unique_ptr<ShaderUniformDescriptor>
    ofImages(Application* application, uint32_t amount);

    // endregion

};

#endif //NEON_SHADERUNIFORMDESCRIPTOR_H
