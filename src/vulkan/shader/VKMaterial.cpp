//
// Created by grial on 22/11/22.
//

#include "VKMaterial.h"

#include <algorithm>
#include <cstring>

#include <engine/Room.h>
#include <engine/shader/Material.h>

#include <vulkan/util/VKUtil.h>

VKMaterial::VKMaterial(
        Room* room, Material* material,
        const InputDescription& vertexDescription,
        const InputDescription& instanceDescription) :
        _material(material),
        _vkApplication(&room->getApplication()->getImplementation()),
        _pipelineLayout(VK_NULL_HANDLE),
        _pipeline(VK_NULL_HANDLE),
        _pushConstants(),
        _pushConstantStages(0) {

    std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
    };

    auto [vBinding, vAttributes] = vulkan_util::toVulkanDescription(
            0, 0, vertexDescription);

    auto [iBinding, iAttributes] = vulkan_util::toVulkanDescription(
            1, vertexDescription.attributes.size(), instanceDescription);

    VkVertexInputBindingDescription bindings[] = {vBinding, iBinding};

    std::vector<VkVertexInputAttributeDescription> attributes;
    attributes.insert(attributes.end(), vAttributes.begin(), vAttributes.end());
    attributes.insert(attributes.end(), iAttributes.begin(), iAttributes.end());


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
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
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

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional
    depthStencil.stencilTestEnable = VK_FALSE;
    depthStencil.front = {}; // Optional
    depthStencil.back = {}; // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional


    VkDescriptorSetLayout uniformInfos[] = {
            room->getGlobalUniformDescriptor()
                    ->getImplementation().getDescriptorSetLayout(),
            material->getUniformDescriptor()
                    ->getImplementation().getDescriptorSetLayout()
    };


    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 2;
    pipelineLayoutInfo.pSetLayouts = uniformInfos;

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
    pipelineInfo.renderPass = _vkApplication->getRenderPass();
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

void VKMaterial::pushConstant(
        const std::string& name, const void* data, uint32_t size) {
    auto& uniforms = _material->getShader()->getImplementation().getUniforms();
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
    if(_pushConstantStages == 0) return;
    vkCmdPushConstants(
            buffer,
            _pipelineLayout,
            _pushConstantStages,
            0,
            _pushConstants.size(),
            _pushConstants.data()
    );
}
