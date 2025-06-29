//
// Created by gaelr on 28/01/2023.
//

#ifndef NEON_TEXTURECREATEINFO_H
#define NEON_TEXTURECREATEINFO_H

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace neon
{
    class CommandBuffer;

    enum class TextureFormat
    {
        R8,
        R8G8,
        R8G8B8,
        B8G8R8,
        R8G8B8A8,
        R8G8B8A8_SRGB,
        A8R8G8B8,
        B8G8R8A8,
        A8B8G8R8,
        R32FG32FB32F,
        R32FG32FB32FA32F,
        R32F,
        R32FG32F,
        R16FG16F,
        R16FG16FB16F,
        R16FG16FB16FA16F,
        DEPTH24STENCIL8,
        DEPTH32F,
        DEPTH32FSTENCIL8
    };

    enum class TextureFilter
    {
        NEAREST,
        LINEAR,
        CUBIC
    };

    enum class AddressMode
    {
        REPEAT,
        MIRRORED_REPEAT,
        CLAMP_TO_EDGE,
        CLAMP_TO_BORDER,
        MIRROR_CLAMP_TO_EDGE
    };

    enum class TextureDimensions
    {
        ONE,
        TWO,
        THREE
    };

    enum class TextureUsage
    {
        TRANSFER_SOURCE = 0x00000001,
        TRANSFER_DESTINY = 0x00000002,
        SAMPLING = 0x00000004,
        STORAGE = 0x00000008,
        COLOR_ATTACHMENT = 0x00000010,
        DEPTH_STENCIL_ATTACHMENT = 0x00000020,
        TRANSFER_ATTACHMENT = 0x00000040,
        INPUT_ATTACHMENT = 0x00000080,
        VIDEO_DECODE_DESTINY = 0x00000400,
        VIDEO_DECODE_SOURCE = 0x00000800,
        VIDEO_DECODE_DPB = 0x00001000,
        FRAGMENT_DESTINY_MAP = 0x00000200,
        FRAGMENT_SHADING_RATE_ATTACHMENT = 0x00000100,
        ATTACHMENT_FEEDBACK_LOOP = 0x00080000,
        INVOCATION_MARK = 0x00040000,
        SAMPLE_WEIGHT = 0x00100000,
        SAMPLE_BLOCK_MATCH = 0x00200000,
    };

    enum class TextureViewType
    {
        NORMAL_1D,
        NORMAL_2D,
        NORMAL_3D,
        CUBE,
        ARRAY_1D,
        ARRAY_2D,
        CUBE_ARRAY
    };

    enum class SamplesPerTexel
    {
        COUNT_1,
        COUNT_2,
        COUNT_4,
        COUNT_8,
        COUNT_16,
        COUNT_32,
        COUNT_64,
    };

    enum class Tiling
    {
        OPTIMAL,
        LINEAR,
        DRM
    };

    enum class TextureComponentSwizzle
    {
        IDENTITY,
        ZERO,
        ONE,
        R,
        G,
        B,
        A
    };

    enum class MipmapMode
    {
        NEAREST,
        LINEAR
    };

    enum class ViewAspect
    {
        COLOR = 0x00000001,
        DEPTH = 0x00000002,
        STENCIL = 0x00000004,
        METADATA = 0x00000008,
        PLANE_0 = 0x00000010,
        PLANE_1 = 0x00000020,
        PLANE_2 = 0x00000040,
        MEMORY_PLANE_0 = 0x00000080,
        MEMORY_PLANE_1 = 0x00000100,
        MEMORY_PLANE_2 = 0x00000200,
        MEMORY_PLANE_3 = 0x00000400,
        ALL = 0x7FFFFFFF
    };

    struct ImageCreateInfo
    {
        /**
         * The format of the texture.
         */
        TextureFormat format = TextureFormat::R8G8B8A8;

        /**
         * The dimensions of the texture.
         */
        TextureDimensions dimensions = TextureDimensions::TWO;

        /**
         * The width of the image.
         * If 0, the engine will calculate the size for you (if possible).
         */
        uint32_t width = 0;

        /**
         * The height of the image.
         * If 0, the engine will calculate the size for you (if possible).
         */
        uint32_t height = 0;

        /**
         * The depth of the image (Used in 3D textures).
         * If 0, the engine will calculate the size for you (if possible).
         */
        uint32_t depth = 0;

        /**
         * The number of layers in the texture.
         */
        uint32_t layers = 1;

        /**
         * The tiling arrangement of the texels.
         * Use "OPTIMAL" for higher performance.
         */
        Tiling tiling = Tiling::OPTIMAL;

        /**
         * The number of samples per texel of the texture.
         */
        SamplesPerTexel samples = SamplesPerTexel::COUNT_1;

        /**
         * The number of mipmaps to generate.
         *
         * If this value is 0, the amount will be calculated automatically.
         */
        uint32_t mipmaps = 0;

        /**
         * The view type of the TextureViews that will use this image.
         * This is a hint for the texture creation.
         */
        TextureViewType viewType = TextureViewType::NORMAL_2D;

        /**
         * Describes how a texture can be used.
         */
        std::vector<TextureUsage> usages = {TextureUsage::TRANSFER_SOURCE, TextureUsage::TRANSFER_DESTINY,
                                            TextureUsage::SAMPLING};
    };

    struct ImageViewCreateInfo
    {
        /**
         * The texture format of the view.
         * This format must be compatible with the format of the texture itself.
         * If empty, the format will match the texture format.
         */
        std::optional<TextureFormat> format = {};

        /**
         * The view mode of the texture.
         */
        TextureViewType viewType = TextureViewType::NORMAL_2D;

        /**
         * What channel the view should use as the R component.
         */
        TextureComponentSwizzle rSwizzle = TextureComponentSwizzle::IDENTITY;

        /**
         * What channel the view should use as the G component.
         */
        TextureComponentSwizzle gSwizzle = TextureComponentSwizzle::IDENTITY;

        /**
         * What channel the view should use as the B component.
         */
        TextureComponentSwizzle bSwizzle = TextureComponentSwizzle::IDENTITY;

        /**
         * What channel the view should use as the A component.
         */
        TextureComponentSwizzle aSwizzle = TextureComponentSwizzle::IDENTITY;

        /**
         * The first mipmap level for the view.
         */
        uint32_t baseMipmapLevel = 0;

        /**
         * The amount of mipmaps for the view to use.
         * If set to 0, the view will use all mipmap levels.
         */
        uint32_t mipmapLevelCount = 0;

        /**
         * The first array layer for the view.
         */
        uint32_t baseArrayLayerLevel = 0;

        /**
         * The number of array layers for the view to use.
         * If set to 0, the view will use all layers.
         */
        uint32_t arrayLayerCount = 0;

        /**
         * The image aspects that will be visible to this view.
         * These aspects can be defined as "the elements of the image that the view can access".
         */
        std::vector<ViewAspect> aspect = {ViewAspect::COLOR};
    };

    struct SamplerCreateInfo
    {
        /**
         * The filter used when there's more fragments than texels.
         */
        TextureFilter magnificationFilter = TextureFilter::LINEAR;

        /**
         * The filter used when there's more texels than fragments.
         */
        TextureFilter minificationFilter = TextureFilter::LINEAR;

        /**
         * The address mode used when a shader fetches a texel out of bounds
         * in the u (x) axis.
         */
        AddressMode uAddressMode = AddressMode::CLAMP_TO_EDGE;

        /**
         * The address mode used when a shader fetches a texel out of bounds
         * in the v (y) axis.
         */
        AddressMode vAddressMode = AddressMode::CLAMP_TO_EDGE;

        /**
         * The address mode used when a shader fetches a texel out of bounds
         * in the w (z) axis.
         */
        AddressMode wAddressMode = AddressMode::CLAMP_TO_EDGE;

        /**
         * Enables anisotropy filter.
         */
        bool anisotropy = true;

        /**
         * The anisotropy value clamp.
         * This value is ignore if "anisotropy" is false.
         * If negative, this value will set to "maxSamplerAnisotropy".
         */
        float maxAnisotropy = -1;

        /**
         * If normalized, textures can be fetched using coordinates from
         * 0 to 1.
         * If not, coordinates should be fetched using the texel coordinates.
         */
        bool normalizedCoordinates = true;

        /**
         * The filter used to interpolate mipmaps.
         */
        MipmapMode mipmapMode = MipmapMode::LINEAR;
    };

    /**
     * Information used to create a texture.
     * By default, this info is configured
     * to create an 2D albedo texture.
     */
    struct TextureCreateInfo
    {
        /**
         * The creation information of the image itself.
         */
        ImageCreateInfo image = ImageCreateInfo();

        /**
         * The creation information of the image view.
         */
        ImageViewCreateInfo imageView = ImageViewCreateInfo();

        /**
         * The creation information of the sampler.
         */
        SamplerCreateInfo sampler = SamplerCreateInfo();

        /**
         * Defines the command buffer used to create
         * the texture.
         * If this command buffer is nullptr, the default
         * command buffer will be used.
         */
        const CommandBuffer* commandBuffer = nullptr;
    };

    namespace serialization
    {
        std::optional<TextureFormat> toTextureFormat(std::string s);

        std::optional<TextureFilter> toTextureFilter(std::string s);

        std::optional<AddressMode> toAddressMode(std::string s);

        std::optional<TextureDimensions> toTextureDimension(std::string s);

        std::optional<TextureUsage> toTextureUsage(std::string s);

        std::optional<TextureViewType> toTextureViewType(std::string s);

        std::optional<SamplesPerTexel> toSamplesPerTexel(std::string s);

        std::optional<Tiling> toTiling(std::string s);

        std::optional<TextureComponentSwizzle> toTextureComponentSwizzle(std::string s);

        std::optional<MipmapMode> toMipmapMode(std::string s);
    } // namespace serialization
} // namespace neon

#endif // NEON_TEXTURECREATEINFO_H
