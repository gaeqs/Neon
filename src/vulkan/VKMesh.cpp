//
// Created by grial on 14/11/22.
//

#include "VKMesh.h"

#include <engine/Application.h>
#include <vulkan/VKApplication.h>
#include <vulkan/VKShaderProgram.h>

void VKMesh::createGraphicPipeline(
        VKShaderProgram* shader,
        const IdentifiableCollection<ShaderUniformBuffer>& uniforms) {

    std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkVertexInputBindingDescription bindingDescriptions[] = {
            _bindingDescription, _instancingBindingDescription};

    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
    attributeDescriptions.reserve(_instancingAttributeDescriptions.size() +
                                  _attributeDescriptions.size());
    attributeDescriptions.insert(attributeDescriptions.end(),
                                 _attributeDescriptions.begin(),
                                 _attributeDescriptions.end());
    attributeDescriptions.insert(attributeDescriptions.end(),
                                 _instancingAttributeDescriptions.begin(),
                                 _instancingAttributeDescriptions.end());

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 2;
    vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions;
    vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

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
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
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


    std::vector<VkDescriptorSetLayout> uniformInfos;
    uniformInfos.reserve(uniforms.getSize());
    uniforms.forEach([&uniformInfos](const ShaderUniformBuffer* buffer) {
        auto layout = buffer->getImplementation().getDescriptorSetLayout();
        uniformInfos.push_back(layout);
    });

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = uniforms.getSize();
    pipelineLayoutInfo.pSetLayouts = uniformInfos.data();
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(_vkApplication->getDevice(),
                               &pipelineLayoutInfo, nullptr,
                               &_pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = shader->getShaders().size();
    pipelineInfo.pStages = shader->getShaders().data();


    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr; // Optional
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

void VKMesh::destroyGraphicPipeline() {
    if (_pipeline != VK_NULL_HANDLE) {
        auto device = _vkApplication->getDevice();
        vkDestroyPipeline(device, _pipeline, nullptr);
        vkDestroyPipelineLayout(device, _pipelineLayout, nullptr);
        vkDestroyRenderPass(device, _renderPass, nullptr);
    }
}

VKMesh::VKMesh(Application* application, Material& material) :
        _vkApplication(&application->getImplementation()),
        _material(material),
        _pipelineLayout(VK_NULL_HANDLE),
        _renderPass(VK_NULL_HANDLE),
        _pipeline(VK_NULL_HANDLE),
        _vertexBuffer(),
        _indexBuffer(),
        _indexAmount(0),
        _attributeDescriptions(),
        _instancingAttributeDescriptions(),
        _bindingDescription({}),
        _instancingBindingDescription({}),
        _currentShader(),
        _currentUniformStatus(0),
        _dirty(true) {
}

VKMesh::~VKMesh() {
    destroyGraphicPipeline();
}

void VKMesh::draw(
        VkCommandBuffer commandBuffer,
        VKShaderProgram* shader,
        VkBuffer instancingBuffer,
        uint32_t instancingElements,
        const IdentifiableCollection<ShaderUniformBuffer>& uniforms) {

    if (!_vertexBuffer.has_value()) return;

    _dirty |= _currentShader == _material.getShader()
              || uniforms.getModificationId() != _currentUniformStatus;
    _currentShader = _material.getShader();
    _currentUniformStatus = uniforms.getModificationId();

    if (_dirty) {
        _dirty = false;
        destroyGraphicPipeline();
        createGraphicPipeline(shader, uniforms);
    }

    VkBuffer buffers[] = {_vertexBuffer.value()->getRaw(), instancingBuffer};
    VkDeviceSize offsets[] = {0, 0};

    vkCmdBindPipeline(commandBuffer,
                      VK_PIPELINE_BIND_POINT_GRAPHICS,
                      _pipeline);
    vkCmdBindVertexBuffers(commandBuffer, 0, 2, buffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer,
                         _indexBuffer.value()->getRaw(),
                         0, VK_INDEX_TYPE_UINT32);
    uniforms.forEach([commandBuffer, this](const ShaderUniformBuffer* uniform) {
        uniform->getImplementation().bind(commandBuffer, _pipelineLayout);
    });

    vkCmdDrawIndexed(commandBuffer, _indexAmount, instancingElements, 0, 0, 0);
}
