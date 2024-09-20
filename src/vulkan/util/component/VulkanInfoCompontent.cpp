//
// Created by gaeqs on 18/09/2024.
//

#include "VulkanInfoCompontent.h"

#include <imgui_internal.h>
#include <neon/Application.h>
#include <vulkan/VKApplication.h>

void neon::vulkan::VulkanInfoCompontent::tooltip(const char* desc) {
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort)) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void neon::vulkan::VulkanInfoCompontent::printProperty(const char* title,
    VkPhysicalDeviceType value) {
    const char* text;
    switch (value) {
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            text = "Integrated GPU";
            break;
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            text = "Discrete GPU";
            break;
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            text = "Virtual GPU";
            break;
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            text = "CPU";
            break;
        default:
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
            text = "Other";
            break;
    }
    printProperty(title, text);
}

void neon::vulkan::VulkanInfoCompontent::printProperty(const char* title,
    uint32_t value) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    ImGui::Text(title);
    ImGui::SameLine(300 + window->DC.Indent.x);
    ImGui::Text("%u", value);
}

void neon::vulkan::VulkanInfoCompontent::printPropertyHex(const char* title,
    uint32_t value) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    ImGui::Text(title);
    ImGui::SameLine(300 + window->DC.Indent.x);
    ImGui::Text("%#.2x", value);
}

void neon::vulkan::VulkanInfoCompontent::printProperty(const char* title,
    int32_t value) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    ImGui::Text(title);
    ImGui::SameLine(300 + window->DC.Indent.x);
    ImGui::Text("%d", value);
}

void neon::vulkan::VulkanInfoCompontent::printProperty(const char* title,
    uint64_t value) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    ImGui::Text(title);
    ImGui::SameLine(300 + window->DC.Indent.x);
    ImGui::Text("%lu", value);
}

void neon::vulkan::VulkanInfoCompontent::printProperty(const char* title,
    bool value) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    ImGui::Text(title);
    ImGui::SameLine(300 + window->DC.Indent.x);
    ImGui::Text(value ? "true" : "false");
}

void neon::vulkan::VulkanInfoCompontent::printPropertyBool(const char* title,
    VkBool32 value) {

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    ImGui::Text(title);
    ImGui::SameLine(300 + window->DC.Indent.x);
    ImGui::Text(value ? "true" : "false");
}

void neon::vulkan::VulkanInfoCompontent::printProperty(const char* title,
    float value) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    ImGui::Text(title);
    ImGui::SameLine(300 + window->DC.Indent.x);
    ImGui::Text("%f", value);
}

void neon::vulkan::VulkanInfoCompontent::printProperty(const char* title,
    const char* value) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    ImGui::Text(title);
    ImGui::SameLine(300 + window->DC.Indent.x);
    ImGui::Text(value);
}

void neon::vulkan::VulkanInfoCompontent::printProperty(const char* title,
    const std::string& value) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    ImGui::Text(title);
    ImGui::SameLine(300 + window->DC.Indent.x);
    ImGui::Text(value.c_str());
}

void neon::vulkan::VulkanInfoCompontent::physicalDeviceProperties() const {
    printProperty("API Version", _properties.apiVersion);
    printProperty("Driver Version", _properties.driverVersion);
    printProperty("Vendor ID", _properties.vendorID);
    printProperty("Device ID", _properties.deviceID);
    printProperty("Device Type", _properties.deviceType);
    printProperty("Device Name", _properties.deviceName);

    if (ImGui::CollapsingHeader("Limits")) {
        ImGui::TreePush("");
        physicalDeviceLimits();
        ImGui::TreePop();
    }


    if (ImGui::CollapsingHeader("Extensions")) {
        ImGui::TreePush("");
        physicalDeviceExtensions();
        ImGui::TreePop();
    }

    if (ImGui::CollapsingHeader("Features")) {
        ImGui::TreePush("");
        physicalDeviceFeatures();
        ImGui::TreePop();
    }
}

void neon::vulkan::VulkanInfoCompontent::physicalDeviceLimits() const {
    auto& l = _properties.limits;
    printProperty("Max Image Dimensions",
                  std::format("{} * {} * {}",
                              l.maxImageDimension1D,
                              l.maxImageDimension2D,
                              l.maxImageDimension3D
                  )
    );

    printProperty("Max Image Dimensions (Cube)", l.maxImageDimensionCube);
    printProperty("Max Image Layers", l.maxImageArrayLayers);
    printProperty("Max Texel Buffer Elements", l.maxTexelBufferElements);
    printProperty("Max Uniform Buffer Range", l.maxUniformBufferRange);
    printProperty("Max Storage Buffer Range", l.maxStorageBufferRange);
    printProperty("Max Push Constant Size", l.maxPushConstantsSize);
    printProperty("Max Memory All. Count", l.maxMemoryAllocationCount);
    printProperty("Max Sampler All. Count", l.maxSamplerAllocationCount);
    printProperty("Buffer Image Granularity", l.bufferImageGranularity);
    printProperty("Sparse Address Space Size", l.sparseAddressSpaceSize);
    printProperty("Max Bound Descriptor Sets", l.maxBoundDescriptorSets);
    printProperty("Max Per Stage Resources", l.maxPerStageResources);
    printProperty("Sub Pixel Presicion Bits", l.subPixelPrecisionBits);
    printProperty("Sub Texel Presicion Bits", l.subTexelPrecisionBits);
    printProperty("Mipmap Precision Bits", l.mipmapPrecisionBits);
    printProperty("Max Draw Indexed Index", l.maxDrawIndexedIndexValue);
    printProperty("Max Draw Indirect Count", l.maxDrawIndirectCount);
    printProperty("Max Sampler Lod Bias", l.maxSamplerLodBias);
    printProperty("Max Sampler Anisotropic", l.maxSamplerAnisotropy);
    printProperty("Max Sampler Anisotropic", l.maxSamplerAnisotropy);
    printProperty("Max Color Attachments", l.maxColorAttachments);
    printPropertyHex("Storage Image Sample Counts",
                     l.storageImageSampleCounts);
    printProperty("Max Sample mask Words", l.maxSampleMaskWords);
    printProperty("Max Clip Distances", l.maxClipDistances);
    printProperty("Max Cull Distances", l.maxCullDistances);
    printProperty("Max Combined Distances",
                  l.maxCombinedClipAndCullDistances);
    printProperty("Discrete Queue Priorities", l.discreteQueuePriorities);

    printProperty("Point Size Range",
                  std::format("{} - {}",
                              l.pointSizeRange[0],
                              l.pointSizeRange[1]
                  )
    );

    printProperty("Line Width Range",
                  std::format("{} - {}",
                              l.lineWidthRange[0],
                              l.lineWidthRange[1]
                  )
    );

    printProperty("Point Size Granularity", l.pointSizeGranularity);
    printProperty("Line Width Granularity", l.lineWidthGranularity);
    printProperty("Strict Lines", static_cast<bool>(l.strictLines));
    printProperty("Standard Sample Locations",
                  static_cast<bool>(l.standardSampleLocations));
    printProperty("Non coherent Atom Size", l.nonCoherentAtomSize);

    ImGui::NewLine();
    ImGui::Text("Per Stage Descriptor:");
    printProperty("  Max Samplers", l.maxPerStageDescriptorSamplers);
    printProperty("  Max Uniform Buffers",
                  l.maxPerStageDescriptorUniformBuffers);
    printProperty("  Max Storage Buffers",
                  l.maxPerStageDescriptorStorageBuffers);
    printProperty("  Max Sampled Images",
                  l.maxPerStageDescriptorSampledImages);
    printProperty("  Max Storage Images",
                  l.maxPerStageDescriptorStorageImages);
    printProperty("  Max Input Attachments",
                  l.maxPerStageDescriptorInputAttachments);

    ImGui::NewLine();
    ImGui::Text("Per Descriptor Set:");
    printProperty("  Max Samplers", l.maxDescriptorSetSamplers);
    printProperty("  Max Uniform Buffers",
                  l.maxDescriptorSetUniformBuffers);
    printProperty("  Max Uniform Buffers (Dyn)",
                  l.maxDescriptorSetUniformBuffersDynamic);
    printProperty("  Max Storage Buffers",
                  l.maxDescriptorSetStorageBuffers);
    printProperty("  Max Storage Buffers (Dyn)",
                  l.maxDescriptorSetStorageBuffersDynamic);
    printProperty("  Max Sampled Images", l.maxDescriptorSetSampledImages);
    printProperty("  Max Storage Images", l.maxDescriptorSetStorageImages);
    printProperty("  Max Input Attachments",
                  l.maxDescriptorSetInputAttachments);

    ImGui::NewLine();
    ImGui::Text("Vertex Shader:");
    printProperty("  Max Input Attributes", l.maxVertexInputAttributes);
    printProperty("  Max Input Bindings", l.maxVertexInputBindings);
    printProperty("  Max Input Att. Offset",
                  l.maxVertexInputAttributeOffset);
    printProperty("  Max Input Binding Stride",
                  l.maxVertexInputBindingStride);
    printProperty("  Max Output Components", l.maxVertexOutputComponents);

    ImGui::NewLine();
    ImGui::Text("Tessellation Control:");
    printProperty("  Max Per Vertex In. Comp.",
                  l.maxTessellationControlPerVertexInputComponents);
    printProperty("  Max Per Vertex Out. Comp.",
                  l.maxTessellationControlPerVertexOutputComponents);
    printProperty("  Max Per Patch Out. Comp.",
                  l.maxTessellationControlPerPatchOutputComponents);
    printProperty("  Max Total Output Comp.",
                  l.maxTessellationControlTotalOutputComponents);

    ImGui::NewLine();
    ImGui::Text("Tessellation Evaluation:");
    printProperty("  Max Input Components",
                  l.maxTessellationEvaluationInputComponents);
    printProperty("  Max Output Components",
                  l.maxTessellationEvaluationOutputComponents);

    ImGui::NewLine();
    ImGui::Text("Geometry Shader:");
    printProperty("  Max Invocations", l.maxGeometryShaderInvocations);
    printProperty("  Max Input Components", l.maxGeometryInputComponents);
    printProperty("  Max Output Components", l.maxGeometryOutputComponents);
    printProperty("  Max Output Vertices", l.maxGeometryOutputVertices);
    printProperty("  Max Total Output Comp.",
                  l.maxGeometryTotalOutputComponents);
    tooltip("The maximum amount of components present in all vertices"
        " generated by the shader invocation.\n"
        "A component can be defined as a float.\n"
        "For example, if each vertex contains 4 floats and this"
        " parameter is 1024, only 256 (1024/4) vertices can be generated,"
        " regardless of the parameter 'maximum output vertices'.");

    ImGui::NewLine();
    ImGui::Text("Fragment Shader:");
    printProperty("  Max Input Components", l.maxFragmentInputComponents);
    printProperty("  Max Output Components",
                  l.maxFragmentOutputAttachments);
    printProperty("  Max Dual Src Attachments",
                  l.maxFragmentDualSrcAttachments);
    printProperty("  Max Combined Out. Res.",
                  l.maxFragmentCombinedOutputResources);

    ImGui::NewLine();
    ImGui::Text("Compute Shader:");
    printProperty("  Max Shared Memory Size", l.maxComputeSharedMemorySize);
    printProperty("  Max Work Group Count",
                  std::format("{} * {} * {}",
                              l.maxComputeWorkGroupCount[0],
                              l.maxComputeWorkGroupCount[1],
                              l.maxComputeWorkGroupCount[2]
                  ));
    printProperty("  Max Work Group Invoc.",
                  l.maxComputeWorkGroupInvocations);
    printProperty("  Max Work Group Size",
                  std::format("{} * {} * {}",
                              l.maxComputeWorkGroupSize[0],
                              l.maxComputeWorkGroupSize[1],
                              l.maxComputeWorkGroupSize[2]
                  ));

    ImGui::NewLine();
    ImGui::Text("Viewport:");
    printProperty("  Max Amount", l.maxViewports);
    printProperty("  Max Dimension",
                  std::format("{} * {}",
                              l.maxViewportDimensions[0],
                              l.maxViewportDimensions[1]
                  ));
    printProperty("  Bounds Range",
                  std::format("{} - {}",
                              l.viewportBoundsRange[0],
                              l.viewportBoundsRange[1]
                  ));
    printProperty("  Sub Pixel Bits", l.viewportSubPixelBits);

    ImGui::NewLine();
    ImGui::Text("Min Alignment:");
    printProperty("  Memory Map", l.minMemoryMapAlignment);
    printProperty("  Texel Buffer Offset", l.minTexelBufferOffsetAlignment);
    printProperty("  Uniform Buffer Offset",
                  l.minUniformBufferOffsetAlignment);
    printProperty("  Storage Buffer Offset",
                  l.minStorageBufferOffsetAlignment);

    ImGui::NewLine();
    ImGui::Text("Texel:");
    printProperty("  Min Offset", l.minTexelOffset);
    printProperty("  Max Offset", l.maxTexelOffset);
    printProperty("  Min Gather Offset", l.minTexelGatherOffset);
    printProperty("  Min Gather Offset", l.maxTexelGatherOffset);

    ImGui::NewLine();
    ImGui::Text("Interpolation:");
    printProperty("  Min Offset", l.minInterpolationOffset);
    printProperty("  Max Offset", l.maxInterpolationOffset);
    printProperty("  Sub Pixel Offset Bits",
                  l.subPixelInterpolationOffsetBits);

    ImGui::NewLine();
    ImGui::Text("Framebuffer:");
    printProperty("  Max Width", l.maxFramebufferWidth);
    printProperty("  Max Height", l.maxFramebufferHeight);
    printProperty("  Max Layers", l.maxFramebufferLayers);
    printPropertyHex("  Color Sample Counts",
                     l.framebufferColorSampleCounts);
    printPropertyHex("  Depth Sample Counts",
                     l.framebufferDepthSampleCounts);
    printPropertyHex("  Stencil Sample Counts",
                     l.framebufferStencilSampleCounts);
    printPropertyHex("  No Attach. Sample Counts",
                     l.framebufferNoAttachmentsSampleCounts);

    ImGui::NewLine();
    ImGui::Text("Sampled Image:");
    printPropertyHex("  Color Sample Counts",
                     l.sampledImageColorSampleCounts);
    printPropertyHex("  Integer Sample Counts",
                     l.sampledImageIntegerSampleCounts);
    printPropertyHex("  Depth Sample Counts",
                     l.sampledImageDepthSampleCounts);
    printPropertyHex("  Stencil Sample Counts",
                     l.sampledImageStencilSampleCounts);

    ImGui::NewLine();
    ImGui::Text("Timestamp:");
    printProperty("  Compute And Graphics",
                  static_cast<bool>(l.timestampComputeAndGraphics));
    printProperty("  Period", l.timestampPeriod);

    ImGui::NewLine();
    ImGui::Text("Optimal buffer:");
    printProperty("  Copy Offset Alignment",
                  l.optimalBufferCopyOffsetAlignment);
    printProperty("  Copy Row Pitch Alighnment",
                  l.optimalBufferCopyRowPitchAlignment);
}

void neon::vulkan::VulkanInfoCompontent::physicalDeviceExtensions() const {
    for (const auto& [name, version]: _extensions) {
        ImGui::Text(name);
        ImGui::SameLine(400);
        ImGui::Text("%u", version);
    }
}

void neon::vulkan::VulkanInfoCompontent::physicalDeviceFeatures() const {
    if (ImGui::CollapsingHeader("Vulkan 1.0")) {
        ImGui::TreePush("");
        const auto& f = _features->getFeatures().features;
        printPropertyBool("Robust Buffer Access", f.robustBufferAccess);
        printPropertyBool("Full Draw Index Uint32", f.fullDrawIndexUint32);
        printPropertyBool("Image Cube Array", f.imageCubeArray);
        printPropertyBool("Independent Blend", f.independentBlend);
        printPropertyBool("Geometry Shader", f.geometryShader);
        printPropertyBool("Tessellation Shader", f.tessellationShader);
        printPropertyBool("Sample Rate Shading", f.sampleRateShading);
        printPropertyBool("Dual Src Blend", f.dualSrcBlend);
        printPropertyBool("Logic Op", f.logicOp);
        printPropertyBool("Multi Draw Indirect", f.multiDrawIndirect);
        printPropertyBool("Draw Indirect First Instance",
                          f.drawIndirectFirstInstance);
        printPropertyBool("Depth Clamp", f.depthClamp);
        printPropertyBool("Depth Bias Clamp", f.depthBiasClamp);
        printPropertyBool("Fill Mode Non Solid", f.fillModeNonSolid);
        printPropertyBool("Depth Bounds", f.depthBounds);
        printPropertyBool("Wide Lines", f.wideLines);
        printPropertyBool("Large Points", f.largePoints);
        printPropertyBool("Alpha To One", f.alphaToOne);
        printPropertyBool("Multi Viewport", f.multiViewport);
        printPropertyBool("Sampler Anisotropy", f.samplerAnisotropy);
        printPropertyBool("Texture Compression ETC2", f.textureCompressionETC2);
        printPropertyBool("Texture Compression ASTC_LDR",
                          f.textureCompressionASTC_LDR);
        printPropertyBool("Texture Compression ASTC_LDR",
                          f.textureCompressionASTC_LDR);
        printPropertyBool("Texture Compression BC", f.textureCompressionBC);
        printPropertyBool("Occlusion Query Precise", f.occlusionQueryPrecise);
        printPropertyBool("Pipeline Statistics Query",
                          f.pipelineStatisticsQuery);
        printPropertyBool("Vertex Pipeline Stores And Atomics",
                          f.vertexPipelineStoresAndAtomics);
        printPropertyBool("Fragment Stores And Atomics",
                          f.fragmentStoresAndAtomics);
        printPropertyBool("Shader Tess./Geom. Point Size",
                          f.shaderTessellationAndGeometryPointSize);
        printPropertyBool("Shader Image Gather Extended",
                          f.shaderImageGatherExtended);
        printPropertyBool("Shader Storage Image Extended Formats",
                          f.shaderStorageImageExtendedFormats);
        printPropertyBool("Shader Storage Image Multisample",
                          f.shaderStorageImageMultisample);
        printPropertyBool("Shader Storage Image Read Without Format",
                          f.shaderStorageImageReadWithoutFormat);
        printPropertyBool("Shader Storage Image Write Without Format",
                          f.shaderStorageImageWriteWithoutFormat);
        printPropertyBool("Shader Uniform Buffer Array Dyn. Indexing",
                          f.shaderUniformBufferArrayDynamicIndexing);
        printPropertyBool("Shader Sampled Image Array Dyn. Indexing",
                          f.shaderSampledImageArrayDynamicIndexing);
        printPropertyBool("Shader Storage Buffer Array Dyn. Indexing",
                          f.shaderStorageBufferArrayDynamicIndexing);
        printPropertyBool("Shader Storage Image Array Dyn. Indexing",
                          f.shaderStorageImageArrayDynamicIndexing);
        printPropertyBool("Shader Clip Distance", f.shaderClipDistance);
        printPropertyBool("Shader Cull Distance", f.shaderCullDistance);
        printPropertyBool("Shader Float64", f.shaderFloat64);
        printPropertyBool("Shader Int64", f.shaderInt64);
        printPropertyBool("Shader Int16", f.shaderInt16);
        printPropertyBool("Shader Resource Residency",
                          f.shaderResourceResidency);
        printPropertyBool("Shader Resource Min Load", f.shaderResourceMinLod);
        printPropertyBool("Sparse Binding", f.sparseBinding);
        printPropertyBool("Sparse Residency Buffer", f.sparseResidencyBuffer);
        printPropertyBool("Sparse Residency Image2D", f.sparseResidencyImage2D);
        printPropertyBool("Sparse Residency Image3D", f.sparseResidencyImage3D);
        printPropertyBool("Sparse Residency 2 Samples",
                          f.sparseResidency2Samples);
        printPropertyBool("Sparse Residency 4 Samples",
                          f.sparseResidency4Samples);
        printPropertyBool("Sparse Residency 8 Samples",
                          f.sparseResidency8Samples);
        printPropertyBool("Sparse Residency 16 Samples",
                          f.sparseResidency16Samples);
        printPropertyBool("Sparse Residency Aliased", f.sparseResidencyAliased);
        printPropertyBool("Variable Multisample Rate",
                          f.variableMultisampleRate);
        printPropertyBool("Inherithed Queries", f.inheritedQueries);
        ImGui::TreePop();
    }

    if (ImGui::CollapsingHeader("Vulkan 1.1")) {
        ImGui::TreePush("");
        const auto& f = _features->getVulkan11Features();
        printPropertyBool("Storage Buffer 16 Bit Access",
                          f.storageBuffer16BitAccess);
        printPropertyBool("Uniform/Storage Buffer 16 Bit Access",
                          f.uniformAndStorageBuffer16BitAccess);
        printPropertyBool("Storage Push Constant 16", f.storagePushConstant16);
        printPropertyBool("Storage Input Output 16", f.storageInputOutput16);
        printPropertyBool("Multiview", f.multiview);
        printPropertyBool("Multiview Geometry Shader",
                          f.multiviewGeometryShader);
        printPropertyBool("Multiview Tessellation Shader",
                          f.multiviewTessellationShader);
        printPropertyBool("Variable Pointers Storage Buffer",
                          f.variablePointersStorageBuffer);
        printPropertyBool("Variable Pointers", f.variablePointers);
        printPropertyBool("Protected Memory", f.protectedMemory);
        printPropertyBool("Sampler Ycbcr Conversion", f.samplerYcbcrConversion);
        printPropertyBool("Shader Draw Parameters", f.shaderDrawParameters);
        ImGui::TreePop();
    }

    if (ImGui::CollapsingHeader("Vulkan 1.2")) {
        ImGui::TreePush("");
        const auto& f = _features->getVulkan12Features();
        printPropertyBool("Sampler Mirror Clamp To Edge",
                          f.samplerMirrorClampToEdge);
        printPropertyBool("Draw Indirect Count", f.drawIndirectCount);
        printPropertyBool("Storage Buffer 8 Bit Access",
                          f.storageBuffer8BitAccess);
        printPropertyBool("Uniform/Storage Buffer 8 Bit Access",
                          f.uniformAndStorageBuffer8BitAccess);
        printPropertyBool("Storage Push Constant 8", f.storagePushConstant8);
        printPropertyBool("Shader Buffer Int64 Atomics",
                          f.shaderBufferInt64Atomics);
        printPropertyBool("Shader Shared Int64 Atomics",
                          f.shaderSharedInt64Atomics);
        printPropertyBool("Shader Float16", f.shaderFloat16);
        printPropertyBool("Shader Int8", f.shaderInt8);
        printPropertyBool("Descriptor Indexing", f.descriptorIndexing);
        printPropertyBool("Shader Input Att. Array Dyn. Indexing",
                          f.shaderInputAttachmentArrayDynamicIndexing);
        printPropertyBool("S. Uniform Texel Buffer Array Dyn. Ind.",
                          f.shaderUniformTexelBufferArrayDynamicIndexing);
        printPropertyBool("S. Storage Texel Buffer Array Dyn. Ind.",
                          f.shaderStorageTexelBufferArrayDynamicIndexing);
        printPropertyBool("S. Uniform Buffer Array Non Uniform Ind.",
                          f.shaderUniformBufferArrayNonUniformIndexing);
        printPropertyBool("S. Sampled Image Array Non Uniform Ind.",
                          f.shaderSampledImageArrayNonUniformIndexing);
        printPropertyBool("S. Sorage Buffer Array Non Uniform Ind.",
                          f.shaderStorageBufferArrayNonUniformIndexing);
        printPropertyBool("S. Sorage Image Array Non Uniform Ind.",
                          f.shaderStorageImageArrayNonUniformIndexing);
        printPropertyBool("S. Input Attach. Array Non Uniform Ind.",
                          f.shaderInputAttachmentArrayNonUniformIndexing);
        printPropertyBool("S. Uniform Texel B. Array Non Un. Ind.",
                          f.shaderUniformTexelBufferArrayNonUniformIndexing);
        printPropertyBool("S. Storage Texel B. Array Non Un. Ind.",
                          f.shaderStorageTexelBufferArrayNonUniformIndexing);
        printPropertyBool("Desc. Bind. Uniform B. Up. After Bind",
                          f.descriptorBindingUniformBufferUpdateAfterBind);
        printPropertyBool("Desc. Bind. Sampled Img. Up. After Bind",
                          f.descriptorBindingSampledImageUpdateAfterBind);
        printPropertyBool("Desc. Bind. Storage Img. Up. After Bind",
                          f.descriptorBindingStorageImageUpdateAfterBind);
        printPropertyBool("Desc. Bind. Storage Buf. Up. After Bind",
                          f.descriptorBindingStorageBufferUpdateAfterBind);
        printPropertyBool("Desc. Bind. U. Texel Buf. Up. After Bind",
                          f.descriptorBindingUniformTexelBufferUpdateAfterBind);
        printPropertyBool("Desc. Bind. S. Texel Buf. Up. After Bind",
                          f.descriptorBindingStorageTexelBufferUpdateAfterBind);
        printPropertyBool("Desc. Binding Upate Unused While Pending",
                          f.descriptorBindingUpdateUnusedWhilePending);
        printPropertyBool("Desc. Binding Partially Bound",
                          f.descriptorBindingPartiallyBound);
        printPropertyBool("Desc. Binding Variable Descriptor Count",
                          f.descriptorBindingVariableDescriptorCount);
        printPropertyBool("Runtime Descriptor Array", f.runtimeDescriptorArray);
        printPropertyBool("Sampler Filter Minmax", f.samplerFilterMinmax);
        printPropertyBool("Scalar Block Layout", f.scalarBlockLayout);
        printPropertyBool("Imageless Framebuffer", f.imagelessFramebuffer);
        printPropertyBool("Uniform Buffer Standard Layout",
                          f.uniformBufferStandardLayout);
        printPropertyBool("Shader Subgroup Extended Types",
                          f.shaderSubgroupExtendedTypes);
        printPropertyBool("Separated Depth Stencil Layouts",
                          f.separateDepthStencilLayouts);
        printPropertyBool("Host Query Reset", f.hostQueryReset);
        printPropertyBool("Timeline Semaphore", f.timelineSemaphore);
        printPropertyBool("Buffer Device Address", f.bufferDeviceAddress);
        printPropertyBool("Buffer Device Address Capture Replay",
                          f.bufferDeviceAddressCaptureReplay);
        printPropertyBool("Buffer Device Address Multi Device",
                          f.bufferDeviceAddressMultiDevice);
        printPropertyBool("Vulkan Memory Model", f.vulkanMemoryModel);
        printPropertyBool("Vulkan Memory Model Device Scope",
                          f.vulkanMemoryModelDeviceScope);
        printPropertyBool("Vulkan Mem. Model Avail. Visib. Chains",
                          f.vulkanMemoryModelAvailabilityVisibilityChains);
        printPropertyBool("Shader Output Viewport Index",
                          f.shaderOutputViewportIndex);
        printPropertyBool("Shader Output Layer", f.shaderOutputLayer);
        printPropertyBool("Subgroup Broadcast Dynamic ID",
                          f.subgroupBroadcastDynamicId);
        ImGui::TreePop();
    }

    if (ImGui::CollapsingHeader("Vulkan 1.3")) {
        ImGui::TreePush("");
        const auto& f = _features->getVulkan13Features();
        printPropertyBool("Robust Image Access", f.robustImageAccess);
        printPropertyBool("Inline Uniform Block", f.inlineUniformBlock);
        printPropertyBool("Descriptor Binding Inline\nUniform Block Update After Bind", f.descriptorBindingInlineUniformBlockUpdateAfterBind);
        printPropertyBool("Pipeline Creation Cache Control", f.pipelineCreationCacheControl);
        printPropertyBool("Private Data", f.privateData);
        printPropertyBool("Shader Demote To Helper Invocation", f.shaderDemoteToHelperInvocation);
        printPropertyBool("Shader Terminate Invocation", f.shaderTerminateInvocation);
        printPropertyBool("Subgroup Size Control", f.subgroupSizeControl);
        printPropertyBool("Compute Full Subgroups", f.computeFullSubgroups);
        printPropertyBool("Synchronization 2", f.synchronization2);
        printPropertyBool("Texture Compression ASTC_HDR", f.textureCompressionASTC_HDR);
        printPropertyBool("Shader Zero Initialize Workgroup Memory", f.shaderZeroInitializeWorkgroupMemory);
        printPropertyBool("Dynamic Rendering", f.dynamicRendering);
        printPropertyBool("Shader Integer Dot Product", f.shaderIntegerDotProduct);
        printPropertyBool("Maintenance 4", f.maintenance4);
        ImGui::TreePop();
    }

    if (ImGui::CollapsingHeader("Mesh Shader")) {
        ImGui::TreePush("");
        const auto& f = _features->getMeshShaderFeature();
        printPropertyBool("Task Shader", f.taskShader);
        printPropertyBool("Mesh Shader", f.meshShader);
        printPropertyBool("Multiview Mesh Shader", f.multiviewMeshShader);
        printPropertyBool("Primitive Fragment Shading\nRate Mesh Shader", f.primitiveFragmentShadingRateMeshShader);
        printPropertyBool("Mesh Shader Queries", f.meshShaderQueries);
        ImGui::TreePop();
    }
}

void neon::vulkan::VulkanInfoCompontent::onStart() {
    auto application = dynamic_cast<VKApplication*>(
        getApplication()->getImplementation());

    VkPhysicalDevice device = application->getPhysicalDevice();
    vkGetPhysicalDeviceProperties(device, &_properties);


    uint32_t amount;
    vkEnumerateDeviceExtensionProperties(device, nullptr,
                                         &amount, nullptr);

    _extensions.resize(amount);
    vkEnumerateDeviceExtensionProperties(device, nullptr,
                                         &amount, _extensions.data());

    _features = &application->getPhysicalDeviceFeatures();
}

void neon::vulkan::VulkanInfoCompontent::onPreDraw() {
    ImGui::ShowDemoWindow();
    if (ImGui::Begin("Vulkan info")) {
        if (ImGui::CollapsingHeader("Physical Device Properties")) {
            ImGui::TreePush("");
            physicalDeviceProperties();
            ImGui::TreePop();
        }
    }
    ImGui::End();
}
