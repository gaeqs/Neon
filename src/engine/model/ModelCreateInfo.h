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
#include <string>
#include <vector>

#include <engine/model/DefaultInstancingData.h>
#include <engine/model/Mesh.h>

namespace neon {

    /**
     * Information used to create a model.
     * Fill an instance of this struct to
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

        /**
         * The optional uniform, descriptor of the model.
         * You must define this descriptor in the materials the
         * model will use.
         */
        std::shared_ptr<ShaderUniformDescriptor> uniformDescriptor = nullptr;

        /**
         * The type of the instance data.
         *
         * Use the method "defineInstanceType()" to change this variable.
         * Don't do it explicitly!
         */
        std::type_index instanceType = typeid(DefaultInstancingData);

        /**
         * The size in bytes of each instance.
         *
         * Use the method "defineInstanceType()" to change this variable.
         * Don't do it explicitly!
         */
        size_t instanceSize = sizeof(DefaultInstancingData);

        template<typename Type>
        void defineInstanceType() {
            instanceType = typeid(Type);
            instanceSize = sizeof(Type);
        }
    };
}

#endif //VISIMPL_MODELCREATEINFO_H
