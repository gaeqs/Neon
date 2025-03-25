//
// Created by gaelr on 9/15/2024.
//

#ifndef INSTANCEDATA_H
#define INSTANCEDATA_H

#include <cstdint>
#include <string>
#include <typeindex>

#include <neon/util/Result.h>

#ifdef USE_VULKAN
    #include <vulkan/render/model/VKInstanceData.h>
#endif

namespace neon
{
    class CommandBuffer;

    /**
     * Base class that manages the instances of a model.
     * Use this class to create, remove and modify model
     * instances.
     * <p>
     * This is a base class: you must provide your model
     * an implementation of this class.
     * The default implementation is BasicInstanceData.
     */
    class InstanceData
    {
      public:
#ifdef USE_VULKAN
        using Implementation = vulkan::VKInstanceData;
#endif

        struct Instance
        {
            uint32_t* id;
        };

        virtual ~InstanceData() = default;

        /**
         * Returns the type of the structure used for instancing data,
         * @return the type.
         */
        [[nodiscard]] virtual const std::vector<std::type_index>& getInstancingStructTypes() const = 0;

        /**
         * Creates an instance of this model.
         * @return the identifier of the instance or the
         * error if something went wrong.
         */
        [[nodiscard]] virtual Result<Instance, std::string> createInstance() = 0;

        /**
        * Creates multiple instances of this model.
        * @return a vector with the identifiers of the instances or the error if something went wrong.
        */
        [[nodiscard]] virtual Result<std::vector<Instance>, std::string> createMultipleInstances(size_t amount) = 0;

        /**
         * Deletes an instance of this model.
         * The instance object used to call this method will be invalid after this call.
         * @param id the identifier of the model to delete.
         * @return whether the operation was successful.
         */
        virtual bool freeInstance(Instance id) = 0;

        /**
         * Deletes multiple instances of this model.
         * All instance objects used to call this method will be invalid after this call.
         * @param ids The vector of instances to delete.
         * @return The number of instances that were successfully deleted.
         */
        virtual size_t freeInstances(const std::vector<Instance>& ids) = 0;

        /**
         * Returns the amount of instances inside this model.
         * @return the amount of instances.
         */
        [[nodiscard]] virtual size_t getInstanceAmount() const = 0;

        /**
         * Sets the instancing data of an instance.
         * @param id the identifier of the instance.
         * @param index the index of the buffer,
         * @param data the data to upload.
         */
        virtual bool uploadData(Instance id, size_t index, const void* data) = 0;

        /**
         * Uploads the instancing data to the GPU.
         * <p>
         * This method is invoked automatically
         * before the room is rendered if shouldAutoFlush() is true.
         * <p>
         * You can provide this method an external command buffer.
         * This allows you to upload the model data asynchronously.
         */
        virtual void flush() = 0;

        /**
         * Uploads the instancing data to the GPU.
         * <p>
         * This method is invoked automatically
         * before the room is rendered if shouldAutoFlush() is true.
         * <p>
         * You can provide this method an external command buffer.
         * This allows you to upload the model data asynchronously.
         */
        virtual void flush(const CommandBuffer* commandBuffer) = 0;

        /**
         * @return the implementation of this structure.
         */
        [[nodiscard]] virtual Implementation& getImplementation() = 0;

        /**
         * @return the implementation of this structure.
         */
        [[nodiscard]] virtual const Implementation& getImplementation() const = 0;

        // Helper templates

        /**
         * Sets the instancing data of an instance.
         * @tparam T the type of the instance data.
         * @param id the identifier of the instance.
         * @param index the index of the instance buffer.
         * @param data the data to upload.
         */
        template<typename T>
        bool uploadData(Instance id, size_t index, const T& data)
        {
            return uploadData(id, index, static_cast<const void*>(&data));
        }
    };
} // namespace neon

#endif //INSTANCEDATA_H
