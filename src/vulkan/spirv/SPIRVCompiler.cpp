//
// Created by grial on 18/11/22.
//

#include "SPIRVCompiler.h"
#include <iostream>

constexpr int DEFAULT_VERSION = 450;

static bool GLSLANG_INITIALIZED = false;


TBuiltInResource SPIRVCompiler::generateDefaultResources() {
    TBuiltInResource resources{};

    resources.maxLights = 32;
    resources.maxClipPlanes = 6;
    resources.maxTextureUnits = 32;
    resources.maxTextureCoords = 32;
    resources.maxVertexAttribs = 64;
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
    resources.maxComputeWorkGroupCountX = 65535;
    resources.maxComputeWorkGroupCountY = 65535;
    resources.maxComputeWorkGroupCountZ = 65535;
    resources.maxComputeWorkGroupSizeX = 1024;
    resources.maxComputeWorkGroupSizeY = 1024;
    resources.maxComputeWorkGroupSizeZ = 64;
    resources.maxComputeUniformComponents = 1024;
    resources.maxComputeTextureImageUnits = 16;
    resources.maxComputeImageUniforms = 8;
    resources.maxComputeAtomicCounters = 8;
    resources.maxComputeAtomicCounterBuffers = 1;
    resources.maxVaryingComponents = 60;
    resources.maxVertexOutputComponents = 64;
    resources.maxGeometryInputComponents = 64;
    resources.maxGeometryOutputComponents = 128;
    resources.maxFragmentInputComponents = 128;
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
    resources.maxGeometryOutputVertices = 256;
    resources.maxGeometryTotalOutputComponents = 1024;
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
    resources.maxViewports = 16;
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
    resources.maxMeshOutputVerticesNV = 256;
    resources.maxMeshOutputPrimitivesNV = 512;
    resources.maxMeshWorkGroupSizeX_NV = 32;
    resources.maxMeshWorkGroupSizeY_NV = 1;
    resources.maxMeshWorkGroupSizeZ_NV = 1;
    resources.maxTaskWorkGroupSizeX_NV = 32;
    resources.maxTaskWorkGroupSizeY_NV = 1;
    resources.maxTaskWorkGroupSizeZ_NV = 1;
    resources.maxMeshViewCountNV = 4;

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

EShLanguage
SPIRVCompiler::getLanguage(const VkShaderStageFlagBits& shaderType) {
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

SPIRVCompiler::SPIRVCompiler() :
        _compiled(false),
        _shaders(),
        _program(),
        _resources(generateDefaultResources()) {
    if (!GLSLANG_INITIALIZED) {
        glslang::InitializeProcess();
        GLSLANG_INITIALIZED = true;
    }
}

SPIRVCompiler::~SPIRVCompiler() {
    for (const auto& item: _shaders) {
        delete item;
    }
}

std::optional<std::string>
SPIRVCompiler::addShader(const VkShaderStageFlagBits& shaderType,
                         const char* source) {
    auto language = getLanguage(shaderType);
    auto* shader = new glslang::TShader(language);

    auto messages = static_cast<EShMessages>(
            EShMsgSpvRules | EShMsgVulkanRules);

    shader->setStrings(&source, 1);

    shader->setEnvInput(glslang::EShSourceGlsl, language,
                        glslang::EShClientVulkan, DEFAULT_VERSION);
    shader->setEnvClient(glslang::EShClientVulkan,
                         glslang::EShTargetVulkan_1_1);
    shader->setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_0);

    if (!shader->parse(&_resources, DEFAULT_VERSION, false, messages)) {
        std::string infoLog(shader->getInfoLog());
        std::string infoDebugLog(shader->getInfoDebugLog());
        delete shader;
        return {infoLog + "\n" + infoDebugLog};
    }
    _shaders.push_back(shader);
    _program.addShader(shader);
    return {};
}

std::optional<std::string> SPIRVCompiler::compile() {
    auto messages = static_cast<EShMessages>(
            EShMsgSpvRules | EShMsgVulkanRules);
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

Result<std::vector<uint32_t>, std::string>
SPIRVCompiler::getStage(const VkShaderStageFlagBits& shaderType) {
    if (!_compiled) {
        return {"Shader is not compiled!"};
    }

    auto* intermediate = _program.getIntermediate(getLanguage(shaderType));
    std::vector<uint32_t> result;
    glslang::GlslangToSpv(*intermediate, result);
    return {result};
}

std::vector<VKShaderUniform> SPIRVCompiler::getUniforms() const {
    std::vector<VKShaderUniform> uniforms;
    uniforms.resize(_program.getNumUniformVariables());
    for (int i = 0; i < _program.getNumUniformVariables(); ++i) {
        auto& current = uniforms[i];
        auto& reflection = _program.getUniform(i);
        current.name = reflection.name;
        current.offset = reflection.offset;
        current.stages = reflection.stages;
    }

    return uniforms;
}
