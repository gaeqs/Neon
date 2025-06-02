//
// Created by gaeqs on 28/05/2025.
//

#ifndef NEON_SAMPLER_H
#define NEON_SAMPLER_H

#include <memory>
#include <neon/structure/Asset.h>
#include <neon/structure/Application.h>
#include "TextureCreateInfo.h"

namespace neon
{
    class Sampler : public Asset
    {
        SamplerCreateInfo _info;

      public:
        Sampler(std::string name, SamplerCreateInfo info);

        [[nodiscard]] const SamplerCreateInfo& getInfo() const;

        [[nodiscard]] virtual void* getNativeHandle() = 0;

        [[nodiscard]] virtual const void* getNativeHandle() const = 0;

        [[nodiscard]] static std::shared_ptr<Sampler> create(Application* application, std::string name,
                                                             const SamplerCreateInfo& info);
    };
} // namespace neon

#endif // NEON_SAMPLER_H
