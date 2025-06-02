//
// Created by grial on 22/11/22.
//

#include "VKMaterial.h"

#include <cstring>

#include <neon/structure/Room.h>
#include <neon/render/shader/Material.h>

#include <utility>
#include <vulkan/render/VKRenderPass.h>

#include <vulkan/util/VKUtil.h>
#include <vulkan/util/VulkanConversions.h>

namespace vc = neon::vulkan::conversions;

namespace neon::vulkan
{
    VKMaterial::VKMaterial(Application* application, Material* material, const MaterialCreateInfo& createInfo) :
        VKResource(application),
        _material(material),
        _pipelineLayout(VK_NULL_HANDLE),
        _pipeline(VK_NULL_HANDLE),
        _pushConstantStages(0),
        _target(createInfo.target->getImplementation().getRenderPass().getRaw())
    {
        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
        };

        std::vector<VkVertexInputAttributeDescription> attributes;
        std::vector<VkVertexInputBindingDescription> bindings;

        for (auto& description : createInfo.descriptions.vertex) {
            auto [binding, att] = vulkan_util::toVulkanDescription(
                static_cast<uint32_t>(bindings.size()), static_cast<uint32_t>(attributes.size()), description);

            bindings.push_back(binding);
            attributes.insert(attributes.end(), att.begin(), att.end());
        }

        for (auto& description : createInfo.descriptions.instance) {
            auto [binding, att] = vulkan_util::toVulkanDescription(
                static_cast<uint32_t>(bindings.size()), static_cast<uint32_t>(attributes.size()), description);

            bindings.push_back(binding);
            attributes.insert(attributes.end(), att.begin(), att.end());
        }

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindings.size());
        vertexInputInfo.pVertexBindingDescriptions = bindings.data();
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size());
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
        rasterizer.frontFace =
            ra.frontFace == FrontFace::COUNTER_CLOCKWISE ? VK_FRONT_FACE_COUNTER_CLOCKWISE : VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f; // Optional
        rasterizer.depthBiasClamp = 0.0f;          // Optional
        rasterizer.depthBiasSlopeFactor = 0.0f;    // Optional

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = conversions::vkSampleCountFlagBits(material->getTarget()->getSamples());
        multisampling.minSampleShading = 1.0f;          // Optional
        multisampling.pSampleMask = nullptr;            // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampling.alphaToOneEnable = VK_FALSE;      // Optional

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
        depthStencil.back = {};  // Optional

        VkPipelineColorBlendAttachmentState defaultColorBlendAttachment{};
        defaultColorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        defaultColorBlendAttachment.blendEnable = false;

        std::vector<VkPipelineColorBlendAttachmentState> blendAttachments;
        blendAttachments.resize(createInfo.target->getImplementation().getColorAttachmentAmount(),
                                defaultColorBlendAttachment);

        size_t maxOutColors = std::min(createInfo.blending.attachmentsBlending.size(), blendAttachments.size());
        for (int i = 0; i < maxOutColors; ++i) {
            auto& att = createInfo.blending.attachmentsBlending[i];
            VkPipelineColorBlendAttachmentState cba{};

            cba.colorWriteMask = att.writeMask;
            cba.blendEnable = att.blend;

            cba.colorBlendOp = vc::vkBlendOp(att.colorBlendOperation);
            cba.srcColorBlendFactor = vc::vkBlendFactor(att.colorSourceBlendFactor);
            cba.dstColorBlendFactor = vc::vkBlendFactor(att.colorDestinyBlendFactor);

            cba.alphaBlendOp = vc::vkBlendOp(att.alphaBlendOperation);
            cba.srcAlphaBlendFactor = vc::vkBlendFactor(att.alphaSourceBlendFactor);
            cba.dstAlphaBlendFactor = vc::vkBlendFactor(att.alphaDestinyBlendFactor);

            blendAttachments.at(i) = cba;
        }

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = createInfo.blending.logicBlending;
        colorBlending.logicOp = vc::vkLogicOp(createInfo.blending.logicOperation);
        colorBlending.attachmentCount = static_cast<uint32_t>(blendAttachments.size());
        colorBlending.pAttachments = blendAttachments.data();
        colorBlending.blendConstants[0] = createInfo.blending.blendingConstants[0];
        colorBlending.blendConstants[1] = createInfo.blending.blendingConstants[1];
        colorBlending.blendConstants[2] = createInfo.blending.blendingConstants[2];
        colorBlending.blendConstants[3] = createInfo.blending.blendingConstants[3];

        // Find a correct descriptor!
        VkDescriptorSetLayout globalDescriptor = nullptr;
        VkDescriptorSetLayout materialDescriptor = nullptr;
        VkDescriptorSetLayout dummyDescriptor = nullptr;

        if (auto& render = application->getRender(); render != nullptr) {
            if (auto desc = render->getGlobalUniformDescriptor(); desc != nullptr) {
                globalDescriptor = desc->getImplementation().getDescriptorSetLayout();
            }
        }

        if (material->getUniformBuffer() != nullptr) {
            materialDescriptor =
                material->getUniformBuffer()->getDescriptor()->getImplementation().getDescriptorSetLayout();
        }

        if (globalDescriptor != nullptr) {
            dummyDescriptor = globalDescriptor;
        } else if (materialDescriptor != nullptr) {
            dummyDescriptor = materialDescriptor;
        } else {
            // Find a dummy descriptor.
            for (const auto& [location, desc] : createInfo.descriptions.uniformBindings | std::views::values) {
                if (location != UniformBufferLocation::EXTRA) {
                    continue;
                }
                if (desc == nullptr) {
                    continue;
                }
                dummyDescriptor = desc->getImplementation().getDescriptorSetLayout();
                break;
            }
        }

        std::vector<VkDescriptorSetLayout> uniformInfos;

        // We can't bind anything if all the descriptors are null!
        if (dummyDescriptor != nullptr && !createInfo.descriptions.uniformBindings.empty()) {
            // Find the max binding location.
            uint8_t max = 0;
            for (const auto& binding : createInfo.descriptions.uniformBindings | std::views::keys) {
                max = std::max(max, binding);
            }

            uniformInfos.resize(max + 1, dummyDescriptor);

            for (const auto& [binding, desc] : createInfo.descriptions.uniformBindings) {
                VkDescriptorSetLayout layout = nullptr;
                switch (desc.location) {
                    case UniformBufferLocation::GLOBAL:
                        layout = globalDescriptor;
                        break;
                    case UniformBufferLocation::MATERIAL:
                        layout = materialDescriptor;
                        break;
                    case UniformBufferLocation::EXTRA:
                        if (desc.extraDescriptor != nullptr) {
                            layout = desc.extraDescriptor->getImplementation().getDescriptorSetLayout();
                        }
                        break;
                    default:
                        break;
                }
                if (layout == nullptr) {
                    continue;
                }
                uniformInfos[binding] = layout;
            }
        }

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(uniformInfos.size());
        pipelineLayoutInfo.pSetLayouts = uniformInfos.data();

        auto& blocks = _material->getShader()->getImplementation().getUniformBlocks();

        std::vector<VkPushConstantRange> pushConstants;

        VkPushConstantRange range;
        range.offset = 0;
        range.size = 0;
        range.stageFlags = 0;

        uint32_t pushStages = 0;
        for (const auto& block : blocks) {
            if (block.binding.has_value()) {
                continue;
            }
            if (!block.offset.has_value()) {
                return;
            }
            VkPushConstantRange range;
            range.offset = block.offset.value();
            range.size = block.sizeInBytes;
            range.stageFlags = block.stages;
            pushStages |= block.stages;
            pushConstants.push_back(range);
        }

        _pushConstants.resize(pushConstants.size(), 0);
        _pushConstantStages = pushStages;

        pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstants.size());
        pipelineLayoutInfo.pPushConstantRanges = pushConstants.data();

        if (vkCreatePipelineLayout(getApplication()->getDevice()->getRaw(), &pipelineLayoutInfo, nullptr,
                                   &_pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout!");
        }

        auto& shaders = material->getShader()->getImplementation().getShaders();

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = static_cast<uint32_t>(shaders.size());
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

        if (vkCreateGraphicsPipelines(getApplication()->getDevice()->getRaw(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr,
                                      &_pipeline) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create graphics pipeline!");
        }
    }

    VKMaterial::~VKMaterial()
    {
        if (_pipeline != VK_NULL_HANDLE) {
            auto device = getApplication()->getDevice()->getRaw();
            auto bin = getApplication()->getBin();
            auto runs = getRuns();
            bin->destroyLater(device, runs, _pipeline, vkDestroyPipeline);
            bin->destroyLater(device, runs, _pipelineLayout, vkDestroyPipelineLayout);
        }
    }

    VkPipelineLayout VKMaterial::getPipelineLayout() const
    {
        return _pipelineLayout;
    }

    VkPipeline VKMaterial::getPipeline() const
    {
        return _pipeline;
    }

    VkRenderPass VKMaterial::getTarget() const
    {
        return _target;
    }

    void VKMaterial::pushConstant(const std::string& name, const void* data, uint32_t size)
    {
        auto& uniforms = _material->getShader()->getUniformBlocks();

        auto uniformIt = std::find_if(uniforms.begin(), uniforms.end(),
                                      [&name](const ShaderUniformBlock& block) { return block.name == name; });

        if (uniformIt == uniforms.end()) {
            return;
        }
        auto& uniform = *uniformIt;

        if (!uniform.offset.has_value()) {
            return;
        }

        // Clamp
        if (uniform.offset >= _pushConstants.size()) {
            return;
        }

        uint32_t from = uniform.offset.value();
        uint32_t to = std::min(from + size, static_cast<uint32_t>(_pushConstants.size()));
        memcpy(_pushConstants.data() + from, data, to - from);
    }

    void VKMaterial::uploadConstants(VkCommandBuffer buffer) const
    {
        if (_pushConstantStages == 0) {
            return;
        }
        vkCmdPushConstants(buffer, _pipelineLayout, _pushConstantStages, 0,
                           static_cast<uint32_t>(_pushConstants.size()), _pushConstants.data());
    }

    void VKMaterial::setTexture(const std::string& name, std::shared_ptr<SampledTexture> texture)
    {
        if (_material->getUniformBuffer() == nullptr) {
            return;
        }
        auto& samplers = _material->getShader()->getUniformSamplers();

        auto samplerIt = std::find_if(samplers.begin(), samplers.end(),
                                      [&name](const ShaderUniformSampler& block) { return block.name == name; });

        if (samplerIt == samplers.end()) {
            return;
        }
        if (samplerIt->binding.has_value()) {
            _material->getUniformBuffer()->setTexture(samplerIt->binding.value(), std::move(texture));
        }
    }

    void VKMaterial::useMaterial(std::shared_ptr<CommandBufferRun> run)
    {
        _material->getShader()->getImplementation().registerRun(run);
        registerRun(std::move(run));
    }
} // namespace neon::vulkan
