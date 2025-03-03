//
// Copyright (c) 2015-2023 VG-Lab/URJC.
//
// Authors: Gael Rial Costas <gael.rial.costas@urjc.es>
//
// This file is part of ViSimpl <https://github.com/vg-lab/visimpl>
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 3.0 as published
// by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

//
// Created by gaelr on 7/07/23.
//

#ifndef VISIMPL_MODELCREATEINFO_H
#define VISIMPL_MODELCREATEINFO_H


#include <cstdint>
#include <memory>
#include <unordered_map>

#include <neon/render/model/DefaultInstancingData.h>
#include <neon/render/model/BasicInstanceData.h>
#include <neon/render/model/Drawable.h>

namespace neon {
    enum class ModelBufferLocation {
        GLOBAL,
        MATERIAL,
        MODEL,
        EXTRA
    };

    struct ModelBufferBinding {
        ModelBufferLocation location = ModelBufferLocation::GLOBAL;
        std::shared_ptr<ShaderUniformBuffer> extraBuffer = nullptr;

        static ModelBufferBinding global() {
            return ModelBufferBinding(ModelBufferLocation::GLOBAL, nullptr);
        }

        static ModelBufferBinding material() {
            return ModelBufferBinding(ModelBufferLocation::MATERIAL, nullptr);
        }

        static ModelBufferBinding model() {
            return ModelBufferBinding(ModelBufferLocation::MODEL, nullptr);
        }

        static ModelBufferBinding extra(std::shared_ptr<ShaderUniformBuffer> descriptor) {
            return ModelBufferBinding(ModelBufferLocation::EXTRA, std::move(descriptor));
        }
    };

    /**
     * Information used to create a model.a gua
     * configure a model.
     */
    struct ModelCreateInfo {
        static constexpr uint32_t DEFAULT_MAXIMUM_INSTANCES = 1024 * 16;

        /**
         * The meshes of the model.
         */
        std::vector<std::shared_ptr<Drawable>> drawables = {};

        /**
         * THe maximum amount of instances the model can hold.
         */
        uint32_t maximumInstances = DEFAULT_MAXIMUM_INSTANCES;

        /**
         * The optional uniform, descriptor of the model.
         * You must define this descriptor in the materials the
         * model will use.
         */
        std::shared_ptr<ShaderUniformDescriptor> uniformDescriptor = nullptr;

        /**
        * Other uniform buffers that can be used by the model.
        * The set index of these buffers start on 3
        * (0 is reserved for the global buffer, 1 is reserved for the material buffer
        * and 2 is reserved for the model buffer).
        */
        std::unordered_map<uint32_t, ModelBufferBinding> uniformBufferBindings = {};

        /**
        * Whether the renderer should call Model::flush() before rendering.
        *
        * Set this flag to false if you want to manage the instance data
        * asyncronally.
        */
        bool shouldAutoFlush = true;

        /**
         * The type of the instance data.
         *
         * Use the method "defineInstanceType()" to change this variable.
         * Don't do it explicitly!
         */
        std::vector<std::type_index> instanceTypes = {
            typeid(DefaultInstancingData)
        };

        /**
         * The size in bytes of each instance.
         *
         * Use the method "defineInstanceType()" to change this variable.
         * Don't do it explicitly!
         */
        std::vector<size_t> instanceSizes = {sizeof(DefaultInstancingData)};

        /**
         * The function that provides the InstanceData of the model.
         * <p>
         * It is recommended to use defineInstanceProvider() to
         * change this parameter.
         * <p>
         * The created instance datas will be managed by the model,
         * no manual deletions are required.
         */
        std::function<std::vector<InstanceData*>(
            Application*, const ModelCreateInfo& info, Model* model)>
        instanceDataProvider
                = [](Application* app, const ModelCreateInfo& info, Model*) {
            return std::vector<InstanceData*>{new BasicInstanceData(app, info)};
        };

        ModelCreateInfo() {
            uniformBufferBindings.insert({0, ModelBufferBinding::global()});
            uniformBufferBindings.insert({1, ModelBufferBinding::material()});
            uniformBufferBindings.insert({2, ModelBufferBinding::model()});
        }

        template<typename... Types>
        void defineInstanceType() {
            instanceTypes.clear();
            instanceSizes.clear();
            ([&] {
                instanceTypes.emplace_back(typeid(Types));
                instanceSizes.push_back(sizeof(Types));
            }(), ...);
        }

        /**
         * Defines a provider that provides the given InstanceData to the model.
         * @tparam IData de InstanceData to provide.
         */
        template<typename IData>
        void defineInstanceProvider() {
            instanceDataProvider = [](Application* app, const ModelCreateInfo& info, Model*) {
                return std::vector<InstanceData*>{new IData(app, info)};
            };
        }
    };
}

#endif //VISIMPL_MODELCREATEINFO_H
