//
// Created by grial on 18/11/22.
//

#include "SPIRVCompiler.h"

#include <neon/render/texture/TextureCreateInfo.h>

#include "glslang/Include/Types.h"
#include "SPIRV/GlslangToSpv.h"

#include <neon/logging/Logger.h>

namespace neon::vulkan
{
    constexpr int DEFAULT_VERSION = 450;

    static bool GLSLANG_INITIALIZED = false;

    std::string* SPIRVIncluder::fetch(std::filesystem::path path)
    {
        if (auto it = _cache.find(path); it != _cache.end()) {
            return &it->second;
        }

        if (auto opt = _fileSystem->readFile(path)) {
            auto& file = opt.value();
            auto [it, ok] = _cache.insert({path, file.toString()});
            return &it->second;
        }

        neon::debug() << "Failed to fetch include file: " << path;
        return nullptr;
    }

    SPIRVIncluder::SPIRVIncluder(FileSystem* fileSystem, std::filesystem::path rootPath) :
        _fileSystem(fileSystem),
        _rootPath(std::move(rootPath))
    {
    }

    glslang::TShader::Includer::IncludeResult* SPIRVIncluder::includeSystem(const char* headerName,
                                                                            const char* includerName, size_t depth)
    {
        auto path = std::filesystem::path(headerName);
        neon::debug() << "Fetching system include file: " << path;
        if (!_fileSystem) {
            return nullptr;
        }

        auto* data = fetch(path);
        if (!data) {
            return nullptr;
        }

        return new IncludeResult(path.generic_string(), data->data(), data->length(), data->data());
    }

    glslang::TShader::Includer::IncludeResult* SPIRVIncluder::includeLocal(const char* headerName,
                                                                           const char* includerName, size_t depth)
    {
        std::filesystem::path path;
        if (includerName) {
            path = std::filesystem::path(includerName).parent_path() / headerName;
        } else {
            path = _rootPath / headerName;
        }

        neon::debug() << "Fetching local include file: " << headerName << " (" << path << ")";
        if (!_fileSystem) {
            return nullptr;
        }

        auto* data = fetch(path);
        if (!data) {
            return nullptr;
        }

        return new IncludeResult(path.generic_string(), data->data(), data->length(), data->data());
    }

    void SPIRVIncluder::releaseInclude(IncludeResult* result)
    {
        delete result;
    }

    TBuiltInResource SPIRVCompiler::generateDefaultResources(const VKPhysicalDevice& device)
    {
        auto& limits = device.getProperties().limits;

        VkPhysicalDeviceMeshShaderPropertiesEXT meshProp = {};
        meshProp.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_PROPERTIES_EXT;
        meshProp.pNext = nullptr;
        VkPhysicalDeviceProperties2 properties2 = {};
        properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        properties2.pNext = &meshProp;
        vkGetPhysicalDeviceProperties2(device.getRaw(), &properties2);

        TBuiltInResource resources{};
        resources.maxLights = 32;
        resources.maxClipPlanes = static_cast<int>(limits.maxClipDistances);
        resources.maxTextureUnits = 32;
        resources.maxTextureCoords = 32;
        resources.maxVertexAttribs = static_cast<int>(limits.maxVertexInputAttributes);
        resources.maxVertexUniformComponents = 4096;
        resources.maxVaryingFloats = 64;
        resources.maxVertexTextureImageUnits = 32;
        resources.maxCombinedTextureImageUnits = 80;
        resources.maxTextureImageUnits = 32;
        resources.maxFragmentUniformComponents = 4096;
        resources.maxDrawBuffers = 32;
        resources.maxVertexUniformVectors = 128;
        resources.maxVaryingVectors = 8;
        resources.maxFragmentUniformVectors = 16;
        resources.maxVertexOutputVectors = 16;
        resources.maxFragmentInputVectors = 15;
        resources.minProgramTexelOffset = -8;
        resources.maxProgramTexelOffset = 7;
        resources.maxClipDistances = 8;
        resources.maxComputeWorkGroupCountX = static_cast<int>(limits.maxComputeWorkGroupCount[0]);
        resources.maxComputeWorkGroupCountY = static_cast<int>(limits.maxComputeWorkGroupCount[1]);
        resources.maxComputeWorkGroupCountZ = static_cast<int>(limits.maxComputeWorkGroupCount[2]);
        resources.maxComputeWorkGroupSizeX = static_cast<int>(limits.maxComputeWorkGroupSize[0]);
        resources.maxComputeWorkGroupSizeY = static_cast<int>(limits.maxComputeWorkGroupSize[1]);
        resources.maxComputeWorkGroupSizeZ = static_cast<int>(limits.maxComputeWorkGroupSize[2]);
        resources.maxComputeUniformComponents = 1024;
        resources.maxComputeTextureImageUnits = 16;
        resources.maxComputeImageUniforms = 8;
        resources.maxComputeAtomicCounters = 8;
        resources.maxComputeAtomicCounterBuffers = 1;
        resources.maxVaryingComponents = 60;
        resources.maxVertexOutputComponents = 64;
        resources.maxGeometryInputComponents = static_cast<int>(limits.maxGeometryInputComponents);
        resources.maxGeometryOutputComponents = static_cast<int>(limits.maxGeometryOutputComponents);
        resources.maxFragmentInputComponents = static_cast<int>(limits.maxFragmentInputComponents);
        resources.maxImageUnits = 8;
        resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
        resources.maxCombinedShaderOutputResources = 8;
        resources.maxImageSamples = 0;
        resources.maxVertexImageUniforms = 0;
        resources.maxTessControlImageUniforms = 0;
        resources.maxTessEvaluationImageUniforms = 0;
        resources.maxGeometryImageUniforms = 0;
        resources.maxFragmentImageUniforms = 8;
        resources.maxCombinedImageUniforms = 8;
        resources.maxGeometryTextureImageUnits = 16;
        resources.maxGeometryOutputVertices = static_cast<int>(limits.maxGeometryOutputVertices);
        resources.maxGeometryTotalOutputComponents = static_cast<int>(limits.maxGeometryTotalOutputComponents);
        resources.maxGeometryUniformComponents = 1024;
        resources.maxGeometryVaryingComponents = 64;
        resources.maxTessControlInputComponents = 128;
        resources.maxTessControlOutputComponents = 128;
        resources.maxTessControlTextureImageUnits = 16;
        resources.maxTessControlUniformComponents = 1024;
        resources.maxTessControlTotalOutputComponents = 4096;
        resources.maxTessEvaluationInputComponents = 128;
        resources.maxTessEvaluationOutputComponents = 128;
        resources.maxTessEvaluationTextureImageUnits = 16;
        resources.maxTessEvaluationUniformComponents = 1024;
        resources.maxTessPatchComponents = 120;
        resources.maxPatchVertices = 32;
        resources.maxTessGenLevel = 64;
        resources.maxViewports = static_cast<int>(limits.maxViewports);
        resources.maxVertexAtomicCounters = 0;
        resources.maxTessControlAtomicCounters = 0;
        resources.maxTessEvaluationAtomicCounters = 0;
        resources.maxGeometryAtomicCounters = 0;
        resources.maxFragmentAtomicCounters = 8;
        resources.maxCombinedAtomicCounters = 8;
        resources.maxAtomicCounterBindings = 1;
        resources.maxVertexAtomicCounterBuffers = 0;
        resources.maxTessControlAtomicCounterBuffers = 0;
        resources.maxTessEvaluationAtomicCounterBuffers = 0;
        resources.maxGeometryAtomicCounterBuffers = 0;
        resources.maxFragmentAtomicCounterBuffers = 1;
        resources.maxCombinedAtomicCounterBuffers = 1;
        resources.maxAtomicCounterBufferSize = 16384;
        resources.maxTransformFeedbackBuffers = 4;
        resources.maxTransformFeedbackInterleavedComponents = 64;
        resources.maxCullDistances = 8;
        resources.maxCombinedClipAndCullDistances = 8;
        resources.maxSamples = 4;

        resources.maxMeshOutputVerticesNV = meshProp.maxMeshOutputVertices;
        resources.maxMeshOutputPrimitivesNV = meshProp.maxMeshOutputPrimitives;
        resources.maxMeshWorkGroupSizeX_NV = meshProp.maxMeshWorkGroupSize[0];
        resources.maxMeshWorkGroupSizeY_NV = meshProp.maxMeshWorkGroupSize[1];
        resources.maxMeshWorkGroupSizeZ_NV = meshProp.maxMeshWorkGroupSize[2];
        resources.maxTaskWorkGroupSizeX_NV = meshProp.maxTaskWorkGroupSize[0];
        resources.maxTaskWorkGroupSizeY_NV = meshProp.maxTaskWorkGroupSize[1];
        resources.maxTaskWorkGroupSizeZ_NV = meshProp.maxTaskWorkGroupSize[2];
        resources.maxMeshViewCountNV = meshProp.maxMeshMultiviewViewCount;

        resources.maxMeshOutputVerticesEXT = meshProp.maxMeshOutputVertices;
        resources.maxMeshOutputPrimitivesEXT = meshProp.maxMeshOutputPrimitives;
        resources.maxMeshWorkGroupSizeX_EXT = meshProp.maxMeshWorkGroupSize[0];
        resources.maxMeshWorkGroupSizeY_EXT = meshProp.maxMeshWorkGroupSize[1];
        resources.maxMeshWorkGroupSizeZ_EXT = meshProp.maxMeshWorkGroupSize[2];
        resources.maxTaskWorkGroupSizeX_EXT = meshProp.maxTaskWorkGroupSize[0];
        resources.maxTaskWorkGroupSizeY_EXT = meshProp.maxTaskWorkGroupSize[1];
        resources.maxTaskWorkGroupSizeZ_EXT = meshProp.maxTaskWorkGroupSize[2];
        resources.maxMeshViewCountEXT = meshProp.maxMeshMultiviewViewCount;

        resources.limits.nonInductiveForLoops = true;
        resources.limits.whileLoops = true;
        resources.limits.doWhileLoops = true;
        resources.limits.generalUniformIndexing = true;
        resources.limits.generalAttributeMatrixVectorIndexing = true;
        resources.limits.generalVaryingIndexing = true;
        resources.limits.generalSamplerIndexing = true;
        resources.limits.generalVariableIndexing = true;
        resources.limits.generalConstantMatrixVectorIndexing = true;

        return resources;
    }

    EShLanguage SPIRVCompiler::getLanguage(const VkShaderStageFlagBits& shaderType)
    {
        switch (shaderType) {
            case VK_SHADER_STAGE_VERTEX_BIT:
                return EShLangVertex;
            case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
                return EShLangTessControl;
            case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
                return EShLangTessEvaluation;
            case VK_SHADER_STAGE_GEOMETRY_BIT:
                return EShLangGeometry;
            case VK_SHADER_STAGE_FRAGMENT_BIT:
                return EShLangFragment;
            case VK_SHADER_STAGE_COMPUTE_BIT:
                return EShLangCompute;
            case VK_SHADER_STAGE_RAYGEN_BIT_NV:
                return EShLangRayGenNV;
            case VK_SHADER_STAGE_ANY_HIT_BIT_NV:
                return EShLangAnyHitNV;
            case VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV:
                return EShLangClosestHitNV;
            case VK_SHADER_STAGE_MISS_BIT_NV:
                return EShLangMissNV;
            case VK_SHADER_STAGE_INTERSECTION_BIT_NV:
                return EShLangIntersectNV;
            case VK_SHADER_STAGE_CALLABLE_BIT_NV:
                return EShLangCallableNV;
            case VK_SHADER_STAGE_TASK_BIT_NV:
                return EShLangTaskNV;
            case VK_SHADER_STAGE_MESH_BIT_NV:
                return EShLangMeshNV;
            default:
                return EShLangVertex;
        }
    }

    SPIRVCompiler::SPIRVCompiler(const VKPhysicalDevice& device, FileSystem* includerFileSystem,
                                 std::filesystem::path includerRootPath) :
        _compiled(false),
        _resources(generateDefaultResources(device)),
        _includer(includerFileSystem, std::move(includerRootPath))
    {
        if (!GLSLANG_INITIALIZED) {
            glslang::InitializeProcess();
            GLSLANG_INITIALIZED = true;
        }
    }

    SPIRVCompiler::~SPIRVCompiler()
    {
        for (const auto& item : _shaders) {
            delete item;
        }
    }

    std::optional<std::string> SPIRVCompiler::addShader(const VkShaderStageFlagBits& shaderType,
                                                        const std::string& source)
    {
        constexpr auto preamble = "#extension GL_GOOGLE_include_directive : enable\n";

        auto language = getLanguage(shaderType);
        auto* shader = new glslang::TShader(language);

        auto messages = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules);

        const char* value = source.data();
        shader->setStrings(&value, 1);
        shader->setPreamble(preamble);
        shader->setEnvInput(glslang::EShSourceGlsl, language, glslang::EShClientVulkan, DEFAULT_VERSION);
        shader->setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_2);
        shader->setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_5);

        if (!shader->parse(&_resources, DEFAULT_VERSION, false, messages, _includer)) {
            std::string infoLog(shader->getInfoLog());
            std::string infoDebugLog(shader->getInfoDebugLog());
            delete shader;
            return {infoLog + "\n" + infoDebugLog};
        }
        _shaders.push_back(shader);
        _program.addShader(shader);
        return {};
    }

    std::optional<std::string> SPIRVCompiler::compile()
    {
        auto messages = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules);
        if (!_program.link(messages)) {
            std::string infoLog(_program.getInfoLog());
            std::string infoDebugLog(_program.getInfoDebugLog());
            return {infoLog + "\n" + infoDebugLog};
        }
        _compiled = true;

        _program.buildReflection();
        _program.dumpReflection();

        return {};
    }

    Result<std::vector<uint32_t>, std::string> SPIRVCompiler::getStage(const VkShaderStageFlagBits& shaderType)
    {
        if (!_compiled) {
            return {"Shader is not compiled!"};
        }

        auto* intermediate = _program.getIntermediate(getLanguage(shaderType));
        std::vector<uint32_t> result;
        glslang::GlslangToSpv(*intermediate, result);
        return {result};
    }

    [[nodiscard]] std::vector<ShaderUniformBlock> SPIRVCompiler::getUniformBlocks() const
    {
        std::vector<ShaderUniformBlock> blocks;

        std::cout << "Buffers: " << _program.getNumBufferBlocks() << std::endl;
        std::cout << "Uniforms: " << _program.getNumUniformBlocks() << std::endl;
        std::cout << "In: " << _program.getNumPipeInputs() << std::endl;
        std::cout << "Out: " << _program.getNumPipeOutputs() << std::endl;

        for (int i = 0; i < _program.getNumPipeInputs(); ++i) {
            auto& it = _program.getPipeInput(i);
            std::cout << " - " << it.name << std::endl;
        }

        for (int i = 0; i < _program.getNumPipeOutputs(); ++i) {
            auto& it = _program.getPipeOutput(i);
            std::cout << " - " << it.name << std::endl;
        }

        for (int i = 0; i < _program.getNumUniformBlocks(); ++i) {
            auto& reflection = _program.getUniformBlock(i);
            auto* type = reflection.getType();

            if (!type->isStruct()) {
                continue;
            }

            auto& qualifier = type->getQualifier();
            auto* structure = type->getStruct();

            ShaderUniformBlock block;
            block.name = reflection.name;
            if (qualifier.hasSet()) {
                block.set = qualifier.layoutSet;
            }
            if (qualifier.hasBinding()) {
                block.binding = qualifier.layoutBinding;
            }
            block.stages = reflection.stages;
            block.sizeInBytes = reflection.size;
            block.offset = reflection.offset;
            block.memberNames.reserve(reflection.numMembers);

            for (size_t m = 0; m < structure->size(); ++m) {
                auto& name = structure->at(m).type->getFieldName();
                block.memberNames.emplace_back(name);
            }

            blocks.emplace_back(block);
        }

        return blocks;
    }

    std::vector<ShaderUniformSampler> SPIRVCompiler::getSamplers() const
    {
        std::vector<ShaderUniformSampler> samplers;
        for (int i = 0; i < _program.getNumUniformVariables(); ++i) {
            auto& reflection = _program.getUniform(i);
            auto* type = reflection.getType();

            if (type->getBasicType() != glslang::TBasicType::EbtSampler) {
                continue;
            }

            auto& qualifier = reflection.getType()->getQualifier();

            ShaderUniformSampler block;
            block.name = reflection.name;
            if (qualifier.hasSet()) {
                block.set = qualifier.layoutSet;
            }
            if (qualifier.hasBinding()) {
                block.binding = qualifier.layoutBinding;
            }
            block.stages = reflection.stages;

            switch (type->getSampler().dim) {
                case glslang::Esd1D:
                    block.type = TextureViewType::NORMAL_1D;
                    break;
                case glslang::Esd2D:
                    block.type = TextureViewType::NORMAL_2D;
                    break;
                case glslang::Esd3D:
                    block.type = TextureViewType::NORMAL_3D;
                    break;
                case glslang::EsdCube:
                    block.type = TextureViewType::CUBE;
                    break;
                case glslang::EsdRect:
                    block.type = TextureViewType::ARRAY_1D;
                    break;
                default:
                    block.type = TextureViewType::NORMAL_2D;
                    break;
            }

            samplers.emplace_back(block);
        }

        return samplers;
    }
} // namespace neon::vulkan
