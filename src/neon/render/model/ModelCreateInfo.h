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
#include <vector>

#include <neon/render/model/DefaultInstancingData.h>
#include <neon/render/model/BasicInstanceData.h>
#include <neon/render/model/Mesh.h>
#include <neon/render/model/Pipeline.h>

namespace neon {
    /**
     * Information used to create a model.a gua
     * configure a model.
     */
    struct ModelCreateInfo {
        static constexpr uint32_t DEFAULT_MAXIMUM_INSTANCES = 1024 * 16;

        /**
         * The meshes of the model.
         */
        std::vector<std::shared_ptr<Mesh>> meshes = {};

        /**
         * THe maximum amount of instances the model can hold.
         */
        uint32_t maximumInstances = DEFAULT_MAXIMUM_INSTANCES;


        Pipeline pipeline = {};

        /**
         * The optional uniform, descriptor of the model.
         * You must define this descriptor in the materials the
         * model will use.
         */
        std::shared_ptr<ShaderUniformDescriptor> uniformDescriptor = nullptr;

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
         */
        std::function<std::unique_ptr<InstanceData>(
            Application*, const ModelCreateInfo& info)> instanceDataProvider
                = [](Application* app, const ModelCreateInfo& info) {
            return std::make_unique<BasicInstanceData>(app, info);
        };

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
            instanceDataProvider = [](Application* app,
                                      const ModelCreateInfo& info) {
                return std::make_unique<IData>(app, info);
            };
        }
    };
}

#endif //VISIMPL_MODELCREATEINFO_H
