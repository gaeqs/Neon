//
// Created by gaelr on 11/02/2023.
//

#ifndef NEON_MATERIALCREATEINFO_H
#define NEON_MATERIALCREATEINFO_H

#include <memory>
#include <utility>
#include <vector>

#include <neon/render/buffer/FrameBuffer.h>
#include <neon/render/shader/ShaderProgram.h>
#include <neon/render/shader/ShaderUniformDescriptor.h>
#include <neon/render/shader/ShaderUniformBuffer.h>
#include <neon/render/model/InputDescription.h>

namespace neon
{
    enum class BlendingLogicOperation
    {
        CLEAR,
        AND,
        AND_REVERSE,
        COPY,
        AND_INVERTED,
        NO_OP,
        XOR,
        OR,
        NOR,
        EQUIVALENT,
        INVERT,
        OR_REVERSE,
        COPY_INVERTED,
        OR_INVERTED,
        NAND,
        SET,
    };

    enum class BlendOperation
    {
        ADD = 0,
        SUBTRACT = 1,
        REVERSE_SUBTRACT = 2,
        MIN = 3,
        MAX = 4
    };

    enum ColorWriteMask
    {
        R = 0x1,
        G = 0x2,
        B = 0x4,
        A = 0x8
    };

    enum class BlendFactor
    {
        ZERO = 0,
        ONE = 1,
        SRC_COLOR = 2,
        ONE_MINUS_SRC_COLOR = 3,
        DST_COLOR = 4,
        ONE_MINUS_DST_COLOR = 5,
        SRC_ALPHA = 6,
        ONE_MINUS_SRC_ALPHA = 7,
        DST_ALPHA = 8,
        ONE_MINUS_DST_ALPHA = 9,
        CONSTANT_COLOR = 10,
        ONE_MINUS_CONSTANT_COLOR = 11,
        CONSTANT_ALPHA = 12,
        ONE_MINUS_CONSTANT_ALPHA = 13,
        SRC_ALPHA_SATURATE = 14,
        SRC1_COLOR = 15,
        ONE_MINUS_SRC1_COLOR = 16,
        SRC1_ALPHA = 17,
        ONE_MINUS_SRC1_ALPHA = 18,
    };

    enum class DepthCompareOperation
    {
        NEVER,
        LESS,
        EQUAL,
        LESS_OR_EQUAL,
        GREATER,
        NOT_EQUAL,
        GREATER_OR_EQUAL,
        ALWAYS,
    };

    enum class PolygonMode
    {
        FILL = 0,
        LINE = 1,
        POINT = 2,
        FILL_RECTANGLE_NVIDIA = 1000153000
    };

    enum class CullMode
    {
        NONE = 0,
        FRONT = 1,
        BACK = 2,
        BOTH = 3
    };

    enum class FrontFace
    {
        COUNTER_CLOCKWISE = 0,
        CLOCKWISE = 1
    };

    enum class PrimitiveTopology
    {
        POINT_LIST = 0,
        LINE_LIST = 1,
        LINE_STRIP = 2,
        TRIANGLE_LIST = 3,
        TRIANGLE_STRIP = 4,
        TRIANGLE_FAN = 5,
        LINE_LIST_WITH_ADJACENCY = 6,
        LINE_STRIP_WITH_ADJACENCY = 7,
        TRIANGLE_LIST_WITH_ADJACENCY = 8,
        TRIANGLE_STRIP_WITH_ADJACENCY = 9,
        PATCH_LIST = 10
    };

    enum class UniformBufferLocation
    {
        GLOBAL,
        MATERIAL,
        EXTRA
    };

    struct DescriptorBinding
    {
        UniformBufferLocation location = UniformBufferLocation::GLOBAL;
        std::shared_ptr<ShaderUniformDescriptor> extraDescriptor = nullptr;

        static DescriptorBinding global()
        {
            return DescriptorBinding(UniformBufferLocation::GLOBAL, nullptr);
        }

        static DescriptorBinding material()
        {
            return DescriptorBinding(UniformBufferLocation::MATERIAL, nullptr);
        }

        static DescriptorBinding extra(std::shared_ptr<ShaderUniformDescriptor> descriptor)
        {
            return DescriptorBinding(UniformBufferLocation::EXTRA, std::move(descriptor));
        }
    };

    struct MaterialDescriptions
    {
        /**
         * The descriptions of the vertex buffers.
         */
        std::vector<InputDescription> vertex = {};

        /**
         * The descriptions of the instance buffers.
         */
        std::vector<InputDescription> instance = {};

        /**
         * The description of the material's uniform buffer.
         * If not null, a new ShaderUniformBuffer will be created according to the given
         * description. This buffer can be used to store information about the material.
         *
         * This description will not be used if "uniformBuffer" is not null, as the material
         * will use the buffer provided by the "uniformBuffer" variable instead.
         */
        std::shared_ptr<ShaderUniformDescriptor> uniform = nullptr;

        /**
         * The ShaderUniformBuffer used by the material. If not null, this
         * buffer will be considered the material's ShaderUniformBuffer.
         * Elsewhere, a new ShaderUniformBuffer will be created using the
         * descriptor provided by the variable "uniform".
         */
        std::shared_ptr<ShaderUniformBuffer> uniformBuffer = nullptr;

        /**
         * The locations where the uniform buffers are bound.
         * You can add extra uniform buffer descriptors to this map.
         */
        std::unordered_map<uint8_t, DescriptorBinding> uniformBindings = {};

        MaterialDescriptions()
        {
            uniformBindings.insert({0, DescriptorBinding::global()});
            uniformBindings.insert({1, DescriptorBinding::material()});
        }
    };

    struct MaterialAttachmentBlending
    {
        /**
         * The color channels the attachment can write to.
         */
        uint32_t writeMask = ColorWriteMask::R | ColorWriteMask::G | ColorWriteMask::B | ColorWriteMask::A;

        /**
         * Whether blending is enabled for the attachment.
         */
        bool blend = false;

        /**
         * The blending operation to perform on the color channel.
         */
        BlendOperation colorBlendOperation = BlendOperation::ADD;

        /**
         * The blend factor applied to the source color.
         */
        BlendFactor colorSourceBlendFactor = BlendFactor::SRC_ALPHA;

        /**
         * The blend factor applied to the destiny color.
         */
        BlendFactor colorDestinyBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA;

        /**
         * The blending operation to perform on the alpha channel.
         */
        BlendOperation alphaBlendOperation = BlendOperation::ADD;

        /**
         * The blend factor applied to the source alpha.
         */
        BlendFactor alphaSourceBlendFactor = BlendFactor::ONE;

        /**
         * The blend factor applied to the destiny alpha.
         */
        BlendFactor alphaDestinyBlendFactor = BlendFactor::ZERO;
    };

    struct MaterialBlending
    {
        /**
         * Whether logic blending operations are enabled.
         */
        bool logicBlending = false;

        /**
         * The logic blending operator.
         */
        BlendingLogicOperation logicOperation = BlendingLogicOperation::CLEAR;

        /**
         * The blending settings for the material's attachments.
         */
        std::vector<MaterialAttachmentBlending> attachmentsBlending;

        /**
         * The blending constants to use in blending operations.
         */
        float blendingConstants[4] = {0};
    };

    struct MaterialDepthStencil
    {
        /**
         * Whether depth test is enabled for the material.
         */
        bool depthTest = true;

        /**
         * Whether depth write is enabled for the material.
         */
        bool depthWrite = true;

        /**
         * The compare operation for the depth test.
         */
        DepthCompareOperation depthCompareOperation = DepthCompareOperation::LESS;

        /**
         * Whether depth
         */
        bool useDepthBounds = false;

        /**
         * The minimum bounds of the depth test.
         */
        float minDepthBounds = 0.0f;

        /**
         * The maximum bounds of the depth test.
         */
        float maxDepthBounds = 1.0f;
    };

    struct MaterialRasterizer
    {
        /**
         * The polygon mode the rasterizer should use.
         */
        PolygonMode polygonMode = PolygonMode::FILL;

        /**
         * The line width of the lines when the polygon mode is in line mode.
         */
        float lineWidth = 1.0f;

        /**
         * Tells the rasterizer the face
         * that should be discarded in a model.
         */
        CullMode cullMode = CullMode::BACK;

        /**
         * Defines how the rasterizer chooses which
         * face is the front face.
         */
        FrontFace frontFace = FrontFace::COUNTER_CLOCKWISE;
    };

    struct MaterialCreateInfo
    {
        /**
         * The target framebuffer for the material.
         * This is the framebuffer where models using
         * the material will be drawn.
         */
        std::shared_ptr<FrameBuffer> target;

        /**
         * The shader the material will use.
         */
        std::shared_ptr<ShaderProgram> shader;

        /**
         * The buffer descriptions for vertex,
         * instance and material uniform data.
         */
        MaterialDescriptions descriptions = MaterialDescriptions();

        /**
         * The blending settings for the
         * material.
         */
        MaterialBlending blending = MaterialBlending();

        /**
         * The depth and stencil settings for the material.
         */
        MaterialDepthStencil depthStencil = MaterialDepthStencil();

        /**
         * The rasterizer settings of the material.
         */
        MaterialRasterizer rasterizer = MaterialRasterizer();

        /**
         * The topology of the primitives.
         */
        PrimitiveTopology topology = PrimitiveTopology::TRIANGLE_LIST;

        MaterialCreateInfo(std::shared_ptr<FrameBuffer> target_, std::shared_ptr<ShaderProgram> shader_) :
            target(std::move(target_)),
            shader(std::move(shader_))
        {
        }
    };
} // namespace neon

#endif // NEON_MATERIALCREATEINFO_H
