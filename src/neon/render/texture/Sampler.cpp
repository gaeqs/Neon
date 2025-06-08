//
// Created by gaeqs on 28/05/2025.
//

#include "Sampler.h"

#ifdef USE_VULKAN
    #include <vulkan/render/texture/VKSampler.h>
#endif

namespace neon
{

    Sampler::Sampler(std::string name, SamplerCreateInfo info) :
        Asset(typeid(Sampler), std::move(name)),
        _info(std::move(info))
    {
    }

    const SamplerCreateInfo& Sampler::getInfo() const
    {
        return _info;
    }

    std::shared_ptr<Sampler> Sampler::create(Application* application, std::string name, const SamplerCreateInfo& info)
    {
#ifdef USE_VULKAN
        return std::make_shared<vulkan::VKSampler>(application, std::move(name), info);
#else
        return nullptr;
#endif
    }
} // namespace neon