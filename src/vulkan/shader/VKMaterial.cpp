//
// Created by grial on 22/11/22.
//

#include "VKMaterial.h"

#include <cstring>

#include <engine/structure/Room.h>
#include <engine/shader/Material.h>

#include <utility>
#include <vulkan/render/VKRenderPass.h>

#include <vulkan/util/VKUtil.h>
#include <vulkan/util/VulkanConversions.h>

namespace vc = neon::vulkan::conversions;

namespace neon::vulkan {
    VKMaterial::VKMaterial(
            Application* application,
            Material* material,
            const MaterialCreateInfo& createInfo) :
            _material(material),
            _vkApplication(dynamic_cast<AbstractVKApplication*>(
                                   application->getImplementation())),
            _pipelineLayout(VK_NULL_HANDLE),
            _pipeline(VK_NULL_HANDLE),
            _pushConstants(),
            _pushConstantStages(0),
            _target(createInfo.target->getImplementation().getRenderPass().getRaw()) {

        std::vector<VkDynamicState> dynamicStates = {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR
        };

        auto [vBinding, vAttributes] = vulkan_util::toVulkanDescription(
                0, 0, createInfo.descriptions.vertex);

        auto [iBinding, iAttributes] = vulkan_util::toVulkanDescription(
                1, createInfo.descriptions.vertex.attributes.size(),
                createInfo.descriptions.instance);

        VkVertexInputBindingDescription bindings[] = {vBinding, iBinding};

        std::vector<VkVertexInputAttributeDescription> attributes;
        attributes.insert(attributes.end(), vAttributes.begin(),
                          vAttributes.end());
        attributes.insert(attributes.end(), iAttributes.begin(),
                          iAttributes.end());


        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 2;
        vertexInputInfo.pVertexBindingDescriptions = bindings;
        vertexInputInfo.vertexAttributeDescriptionCount = attributes.size();
        vertexInputInfo.pVertexAttributeDescriptions = attributes.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = vc::vkPrimitiveTopology(createInfo.topology);
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        auto& ra = createInfo.rasterizer;
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = vc::vkPolygonMode(ra.polygonMode);
        rasterizer.lineWidth = ra.lineWidth;
        rasterizer.cullMode = vc::vkCullModeFlagBits(ra.cullMode);
        rasterizer.frontFace = ra.frontFace == FrontFace::COUNTER_CLOCKWISE
                               ? VK_FRONT_FACE_COUNTER_CLOCKWISE
                               : VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f; // Optional
        rasterizer.depthBiasClamp = 0.0f; // Optional
        rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f; // Optional
        multisampling.pSampleMask = nullptr; // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampling.alphaToOneEnable = VK_FALSE; // Optional

        auto& di = createInfo.depthStencil;
        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = di.depthTest;
        depthStencil.depthWriteEnable = di.depthWrite;
        depthStencil.depthCompareOp = vc::vkCompareOp(di.depthCompareOperation);
        depthStencil.depthBoundsTestEnable = di.useDepthBounds;
        depthStencil.minDepthBounds = di.minDepthBounds;
        depthStencil.maxDepthBounds = di.maxDepthBounds;
        depthStencil.stencilTestEnable = VK_FALSE;
        depthStencil.front = {}; // Optional
        depthStencil.back = {}; // Optional

        VkPipelineColorBlendAttachmentState defaultColorBlendAttachment{};
        defaultColorBlendAttachment.colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        defaultColorBlendAttachment.blendEnable = false;

        std::vector<VkPipelineColorBlendAttachmentState> blendAttachments;
        blendAttachments.resize(
                createInfo.target->getImplementation().getColorAttachmentAmount(),
                defaultColorBlendAttachment
        );

        size_t maxOutColors = std::min(
                createInfo.blending.attachmentsBlending.size(),
                blendAttachments.size());
        for (int i = 0; i < maxOutColors; ++i) {
            auto& att = createInfo.blending.attachmentsBlending[i];
            VkPipelineColorBlendAttachmentState cba{};

            cba.colorWriteMask = att.writeMask;
            cba.blendEnable = att.blend;

            cba.colorBlendOp = vc::vkBlendOp(att.colorBlendOperation);
            cba.srcColorBlendFactor = vc::vkBlendFactor(
                    att.colorSourceBlendFactor);
            cba.dstColorBlendFactor = vc::vkBlendFactor(
                    att.colorDestinyBlendFactor);

            cba.alphaBlendOp = vc::vkBlendOp(att.alphaBlendOperation);
            cba.srcAlphaBlendFactor = vc::vkBlendFactor(
                    att.alphaSourceBlendFactor);
            cba.dstAlphaBlendFactor = vc::vkBlendFactor(
                    att.alphaDestinyBlendFactor);

            blendAttachments.at(i) = cba;
        }

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = createInfo.blending.logicBlending;
        colorBlending.logicOp = vc::vkLogicOp(
                createInfo.blending.logicOperation);
        colorBlending.attachmentCount = blendAttachments.size();
        colorBlending.pAttachments = blendAttachments.data();
        colorBlending.blendConstants[0] = createInfo.blending.blendingConstants[0];
        colorBlending.blendConstants[1] = createInfo.blending.blendingConstants[1];
        colorBlending.blendConstants[2] = createInfo.blending.blendingConstants[2];
        colorBlending.blendConstants[3] = createInfo.blending.blendingConstants[3];

        std::vector<VkDescriptorSetLayout> uniformInfos;
        uniformInfos.reserve(2 + createInfo.descriptions.extraUniforms.size());
        uniformInfos.push_back(
                application->getRender()
                        ->getGlobalUniformDescriptor()
                        ->getImplementation().getDescriptorSetLayout());
        if (material->getUniformBuffer() != nullptr) {
            uniformInfos.push_back(material->getUniformBuffer()
                                           ->getDescriptor()
                                           ->getImplementation()
                                           .getDescriptorSetLayout());
        }

        for (const auto& descriptor: createInfo.descriptions.extraUniforms) {
            uniformInfos.push_back(descriptor->getImplementation()
                                           .getDescriptorSetLayout());
        }

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = uniformInfos.size();
        pipelineLayoutInfo.pSetLayouts = uniformInfos.data();

        auto& blocks = _material->getShader()
                ->getImplementation().getUniformBlocks();

        if (blocks.empty()) {
            pipelineLayoutInfo.pushConstantRangeCount = 0;
            pipelineLayoutInfo.pPushConstantRanges = nullptr;
        } else {
            VkPushConstantRange range;
            range.offset = 0;
            range.size = 0;
            range.stageFlags = 0;

            for (const auto& [name, block]: blocks) {
                range.size = std::max(range.size, block.size);
                range.stageFlags |= block.stages;
            }

            _pushConstants.resize(range.size, 0);
            _pushConstantStages = range.stageFlags;

            pipelineLayoutInfo.pushConstantRangeCount = 1;
            pipelineLayoutInfo.pPushConstantRanges = &range;
        }

        if (vkCreatePipelineLayout(_vkApplication->getDevice(),
                                   &pipelineLayoutInfo, nullptr,
                                   &_pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout!");
        }

        auto& shaders = material->getShader()->getImplementation().getShaders();

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = shaders.size();
        pipelineInfo.pStages = shaders.data();

        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = _pipelineLayout;
        pipelineInfo.renderPass = _target;
        pipelineInfo.subpass = 0;

        if (vkCreateGraphicsPipelines(
                _vkApplication->getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo,
                nullptr, &_pipeline) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create graphics pipeline!");
        }
    }

    VKMaterial::~VKMaterial() {
        if (_pipeline != VK_NULL_HANDLE) {
            auto device = _vkApplication->getDevice();
            vkDestroyPipeline(device, _pipeline, nullptr);
            vkDestroyPipelineLayout(device, _pipelineLayout, nullptr);
        }
    }

    VkPipelineLayout VKMaterial::getPipelineLayout() const {
        return _pipelineLayout;
    }

    VkPipeline VKMaterial::getPipeline() const {
        return _pipeline;
    }

    VkRenderPass VKMaterial::getTarget() const {
        return _target;
    }

    void VKMaterial::pushConstant(
            const std::string& name, const void* data, uint32_t size) {
        auto& uniforms = _material->getShader()->getImplementation()
                .getUniforms();
        auto uniformIt = uniforms.find(name);
        if (uniformIt == uniforms.end()) return;
        auto& uniform = uniformIt->second;

        // Clamp
        if (uniform.offset >= _pushConstants.size()) return;

        uint32_t from = uniform.offset;
        uint32_t to = std::min(from + size, static_cast<uint32_t>(
                _pushConstants.size()));
        memcpy(_pushConstants.data() + from, data, to - from);
    }

    void VKMaterial::uploadConstants(VkCommandBuffer buffer) const {
        if (_pushConstantStages == 0) return;
        vkCmdPushConstants(
                buffer,
                _pipelineLayout,
                _pushConstantStages,
                0,
                _pushConstants.size(),
                _pushConstants.data()
        );
    }

    void VKMaterial::setTexture(const std::string& name,
                                std::shared_ptr<Texture> texture) {
        if (_material->getUniformBuffer() == nullptr) return;
        auto& samplers = _material->getShader()->getImplementation()
                .getSamplers();
        auto samplerIt = samplers.find(name);
        if (samplerIt == samplers.end()) return;

        _material->getUniformBuffer()->setTexture(
                samplerIt->second.binding,
                std::move(texture));
    }
}