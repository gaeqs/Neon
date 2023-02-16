//
// Created by gaelr on 28/01/2023.
//

#include <cstdint>

#ifndef NEON_TEXTURECREATEINFO_H
#define NEON_TEXTURECREATEINFO_H

namespace neon {
    enum class TextureFormat {
        R8G8B8,
        B8G8R8,
        R8G8B8A8,
        R8G8B8A8_SRGB,
        A8R8G8B8,
        B8G8R8A8,
        A8B8G8R8,
        R32FG32FB32FA32F,
        R32F,
        R16FG16F,
        R16FG16FB16F,
        R16FG16FB16FA16F,
        DEPTH24STENCIL8
    };


    enum class TextureFilter {
        NEAREST,
        LINEAR,
        CUBIC
    };

    enum class AddressMode {
        REPEAT,
        MIRRORED_REPEAT,
        CLAMP_TO_EDGE,
        CLAMP_TO_BORDER,
        MIRROR_CLAMP_TO_EDGE
    };

    enum class TextureDimensions {
        ONE,
        TWO,
        THREE
    };

    enum class TextureViewType {
        NORMAL_1D,
        NORMAL_2D,
        NORMAL_3D,
        CUBE,
        ARRAY_1D,
        ARRAY_2D,
        CUBE_ARRAY
    };

    enum class SamplesPerTexel {
        COUNT_1,
        COUNT_2,
        COUNT_4,
        COUNT_8,
        COUNT_16,
        COUNT_32,
        COUNT_64,
    };

    enum class Tiling {
        OPTIMAL,
        LINEAR,
        DRM
    };

    enum class TextureComponentSwizzle {
        IDENTITY,
        ZERO,
        ONE,
        R,
        G,
        B,
        A
    };

    enum class MipmapMode {
        NEAREST,
        LINEAR
    };


    struct ImageCreateInfo {
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
         * The amount of layers of the texture.
         */
        uint32_t layers = 1;

        /**
         * The tiling arrangement of the texels.
         * Use "OPTIMAL" for more performance.
         */
        Tiling tiling = Tiling::OPTIMAL;

        /**
         * The amount of samples per texel of the texture.
         */
        SamplesPerTexel samples = SamplesPerTexel::COUNT_1;

        /**
         * The amount of mipmaps to generate.
         *
         * If this value is 0, the amount will be calculated automatically.
         */
        uint32_t mipmaps = 0;


    };

    struct ImageViewCreateInfo {
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
         * The amount of array layers for the view to use.
         * If set to 0, the view will use all layers.
         */
        uint32_t arrayLayerCount = 0;
    };

    struct SamplerCreateInfo {

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
    struct TextureCreateInfo {

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
    };
}

#endif //NEON_TEXTURECREATEINFO_H