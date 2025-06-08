//
// Created by gaeqs on 27/05/2025.
//

#ifndef NEON_TEXTURE_H
#define NEON_TEXTURE_H

#include <string>
#include <optional>
#include <any>

#include <rush/rush.h>

#include <neon/structure/Application.h>
#include <neon/structure/Asset.h>
#include <neon/render/texture/TextureCreateInfo.h>
#include <neon/filesystem/CMRCFileSystem.h>
#include <neon/util/Result.h>

namespace neon

{
    /**
     * @brief Capability interface for textures that support dynamic data modification.
     *
     * Provides an API to update a region of the texture with new data.
     * This is useful for applications such as streaming textures, dynamic texture painting, or procedural updates.
     */
    class TextureCapabilityModifiable
    {
      public:
        virtual ~TextureCapabilityModifiable() = default;

        /**
         * @brief Updates a region of the texture with new data.
         *
         * @param data Pointer to the source data to upload.
         * @param offset 3D offset in texels where the update should begin.
         * @param size Size of the region to update in texels.
         * @param layerOffset Index of the first texture layer to update.
         * @param layers Number of texture layers to update.
         * @param commandBuffer Optional command buffer used for GPU transfer.
         * @return A Result indicating success or an error message.
         */
        virtual Result<void, std::string> updateData(const std::byte* data, rush::Vec3ui offset, rush::Vec3ui size,
                                                     uint32_t layerOffset, uint32_t layers,
                                                     CommandBuffer* commandBuffer) = 0;
    };

    /**
     * @brief Capability Interface for textures that support reading back data to CPU memory.
     *
     * Allows extracting a region of the texture and storing it into a CPU-accessible buffer.
     */
    class TextureCapabilityRead
    {
      public:
        virtual ~TextureCapabilityRead() = default;

        /**
         * @brief Reads a region of the texture into the given buffer.
         *
         * @param data Destination buffer to receive the pixel data.
         * @param offset 3D offset in texels where the read should begin.
         * @param size Size of the region to read in texels.
         * @param layerOffset Index of the first texture layer to read.
         * @param layers Number of texture layers to read.
         * @return A Result indicating success or an error message.
         */
        virtual Result<void, std::string> readData(std::byte* data, rush::Vec3ui offset, rush::Vec3ui size,
                                                   uint32_t layerOffset, uint32_t layers) const = 0;
    };

    /**
     * @brief Abstract class representing a texture asset in the Neon rendering system.
     *
     * This class provides an interface for textures, encapsulating properties such as dimensions,
     * format, mipmaps, and native resource handles.
     *
     * Textures can be created from raw pixel data or from image files using stb_image for decoding.
     *
     * Textures can be extended using capabilities.
     * By default, textures can provide read and modification capabilities.
     */
    class Texture : public Asset
    {
      public:
        /**
         * @brief Constructs a Texture with a given name identifier.
         *
         * @param name Unique identifier for the texture asset.
         */
        explicit Texture(std::string name);

        ~Texture() override = default;

        /**
         * @brief Retrieves the dimensions of the texture.
         *
         * @return The texture dimensions as a 3-dimensional vector.
         */
        [[nodiscard]] virtual rush::Vec3ui getDimensions() const = 0;

        /**
         * @brief Retrieves the format of the texture.
         *
         * @return The format enumeration of the texture.
         */
        [[nodiscard]] virtual TextureFormat getFormat() const = 0;

        /**
         * @brief Retrieves the sampling rate per texel of the texture.
         * This feature is used for antialiasing.
         *
         * @return The number of samples per texel.
         */
        [[nodiscard]] virtual SamplesPerTexel getSamplesPerTexel() const = 0;

        /**
         * @brief Specifies the view type for creating TextureView instances using this Texture.
         *
         * @return Texture view type enumeration.
         */
        [[nodiscard]] virtual TextureViewType getTextureViewType() const = 0;

        /**
         * @brief Retrieves the number of mipmaps present in the texture.
         *
         * @return The mipmap count.
         */
        [[nodiscard]] virtual size_t getNumberOfMipmaps() const = 0;

        /**
         * @brief Reports the total memory allocated by this texture, measured in bytes.
         *
         * @return The allocated memory size in bytes.
         */
        [[nodiscard]] virtual size_t getAllocatedMemoryInBytes() const = 0;

        /**
         * @brief Attempts to get a readable capability interface for this Texture if supported.
         *
         * @return Optional pointer to a constant readable capability interface.
         */
        [[nodiscard]] virtual std::optional<const TextureCapabilityRead*> asReadable() const = 0;

        /**
         * @brief Attempts to get a modifiable capability interface for this Texture if supported.
         *
         * @return Optional pointer to a modifiable capability interface.
         */
        [[nodiscard]] virtual std::optional<TextureCapabilityModifiable*> asModifiable() = 0;

        /**
         * @brief Retrieves the native handle to the underlying graphics API resource.
         *
         * For example, if the backend is Vulkan, this method returns a VkTexture.
         *
         * @return Pointer to the native resource handle.
         */
        [[nodiscard]] virtual void* getNativeHandle() = 0;

        /**
         * @brief Retrieves the native handle to the underlying graphics API resource.
         *
         * For example, if the backend is Vulkan, this method returns a VkTexture.
         *
         * @return Pointer to the native resource handle.
         */
        [[nodiscard]] virtual const void* getNativeHandle() const = 0;

        /**
         * @brief Retrieves a graphics API-specific layout handle for the texture.
         *
         * For example, if the backend is Vulkan, this method returns a VkImageLayout.
         *
         * @return An object encapsulating the layout native handle.
         */
        [[nodiscard]] virtual std::any getLayoutNativeHandle() const = 0;

        /**
         * @brief Creates a new texture from the provided raw data.
         *
         * The size of the data buffer will be computed by multiplying the dimensions,
         * the number of layers, and the pixel size of the texture.
         *
         * For example, if your texture's dimensions are 32x32x1, it has 2 layers, and it uses
         * the A8R8G8B8 format, the size of the buffer must be:
         * - 32 x 32 x 1 x 2 x 4 = 8192 Bytes.
         *
         * @param application The application used to create this texture.
         * @param name The name of the texture.
         * @param data A pointer to the raw data used to create the texture.
         * @param createInfo The information used to create the texture.
         * @param commandBuffer This optional command buffer will be used to transfer the texture data to the GPU.
         * @return A newly created texture based on the provided raw data.
         */
        static std::shared_ptr<Texture> createFromRawData(Application* application, std::string name, const void* data,
                                                          const TextureCreateInfo& createInfo = TextureCreateInfo(),
                                                          CommandBuffer* commandBuffer = nullptr);

        /**
         * @brief Creates a new texture from an image file using stb_image for decoding.
         *
         * Supports image formats such as PNG, JPG, and others compatible with stb_image.
         * The image file is decoded, and the raw pixel data is used to create the texture.
         *
         * @param application The application used to create this texture.
         * @param name The name of the texture.
         * @param resource A CMRC file containing the image data.
         * @param createInfo The information used to create the texture.
         * @param commandBuffer This optional command buffer will be used to transfer the texture data to the GPU.
         * @return A newly created texture.
         */
        static std::shared_ptr<Texture> createTextureFromFile(Application* application, std::string name,
                                                              const cmrc::file& resource,
                                                              const TextureCreateInfo& createInfo = TextureCreateInfo(),
                                                              CommandBuffer* commandBuffer = nullptr);

        /**
         * @brief Creates a new texture from an image file using stb_image for decoding.
         *
         * Supports image formats such as PNG, JPG, and others compatible with stb_image.
         * The image file is decoded, and the raw pixel data is used to create the texture.
         *
         * @param application The application used to create this texture.
         * @param name The name of the texture.
         * @param resource A file containing the image data.
         * @param createInfo The information used to create the texture.
         * @param commandBuffer This optional command buffer will be used to transfer the texture data to the GPU.
         * @return A newly created texture.
         */
        static std::shared_ptr<Texture> createTextureFromFile(Application* application, std::string name,
                                                              const File& resource,
                                                              const TextureCreateInfo& createInfo = TextureCreateInfo(),
                                                              CommandBuffer* commandBuffer = nullptr);

        /**
         * @brief Creates a new texture from multiple image files using stb_image for decoding.
         *
         * Useful for creating textures such as cube maps or texture arrays from several individual images.
         * Each file is decoded, and their raw pixel data is combined into a single texture.
         * Supports common image formats like PNG, JPG, and others compatible with stb_image.
         *
         * @param application The application used to create this texture.
         * @param name Unique identifier for the texture.
         * @param resources A vector of CMRC files containing the image data.
         * @param createInfo Additional texture creation settings.
         * @param commandBuffer Optional command buffer used for GPU data transfer.
         * @return A newly created texture.
         */
        static std::shared_ptr<Texture> createTextureFromFiles(
            Application* application, std::string name, const std::vector<cmrc::file>& resources,
            const TextureCreateInfo& createInfo = TextureCreateInfo(), CommandBuffer* commandBuffer = nullptr);

        /**
         * @brief Creates a new texture from an image file using stb_image for decoding.
         *
         * Supports image formats such as PNG, JPG, and others compatible with stb_image.
         * The image file is decoded, and the raw pixel data is used to create the texture.
         *
         * @param application The application used to create this texture.
         * @param name The name of the texture.
         * @param path The path to the filesystem file containing the image data.
         * @param createInfo The information used to create the texture.
         * @param commandBuffer This optional command buffer will be used to transfer the texture data to the GPU.
         * @return A newly created texture.
         */
        static std::shared_ptr<Texture> createTextureFromFile(Application* application, std::string name,
                                                              const std::string& path,
                                                              TextureCreateInfo createInfo = TextureCreateInfo(),
                                                              CommandBuffer* commandBuffer = nullptr);
        /**
         * @brief Creates a new texture from multiple image files using stb_image for decoding.
         *
         * Useful for creating textures such as cube maps or texture arrays from several individual images.
         * Each file is decoded, and their raw pixel data is combined into a single texture.
         * Supports common image formats like PNG, JPG, and others compatible with stb_image.
         *
         * @param application The application used to create this texture.
         * @param name Unique identifier for the texture.
         * @param paths A vector of paths to the filesystem files containing the image data.
         * @param createInfo Additional texture creation settings.
         * @param commandBuffer Optional command buffer used for GPU data transfer.
         * @return A newly created texture.
         */
        static std::shared_ptr<Texture> createTextureFromFiles(Application* application, std::string name,
                                                               const std::vector<std::string>& paths,
                                                               TextureCreateInfo createInfo = TextureCreateInfo(),
                                                               CommandBuffer* commandBuffer = nullptr);

        /**
         * @brief Creates a new texture from an image file using stb_image for decoding.
         *
         * Supports image formats such as PNG, JPG, and others compatible with stb_image.
         * The image file is decoded, and the raw pixel data is used to create the texture.
         *
         * @param application The application used to create this texture.
         * @param name The name of the texture.
         * @param data the raw data of the file.
         * @param size the size in bytes of the data buffer.
         * @param createInfo The information used to create the texture.
         * @param commandBuffer This optional command buffer will be used to transfer the texture data to the GPU.
         * @return A newly created texture.
         */
        static std::shared_ptr<Texture> createTextureFromFile(Application* application, std::string name,
                                                              const void* data, uint32_t size,
                                                              TextureCreateInfo createInfo = TextureCreateInfo(),
                                                              CommandBuffer* commandBuffer = nullptr);
        /**
         * @brief Creates a new texture from multiple image files using stb_image for decoding.
         *
         * Useful for creating textures such as cube maps or texture arrays from several individual images.
         * Each file is decoded, and their raw pixel data is combined into a single texture.
         * Supports common image formats like PNG, JPG, and others compatible with stb_image.
         *
         * @param application The application used to create this texture.
         * @param name Unique identifier for the texture.
         * @param data A vector with the raw data of the files containing the image data.
         * @param sizes A vector with the sizes of the buffer provided by the vector "data".
         * @param createInfo Additional texture creation settings.
         * @param commandBuffer Optional command buffer used for GPU data transfer.
         * @return A newly created texture.
         */
        static std::shared_ptr<Texture> createTextureFromFiles(Application* application, std::string name,
                                                               const std::vector<const void*>& data,
                                                               const std::vector<uint32_t>& sizes,
                                                               TextureCreateInfo createInfo = TextureCreateInfo(),
                                                               CommandBuffer* commandBuffer = nullptr);
    };
} // namespace neon

#endif // NEON_TEXTURE_H
