#pragma once

#include "adreno_sampler.h"

namespace hpc
{
namespace backend
{
namespace adreno
{

class AdrenoSamplerA6XX final : public AdrenoSampler
{
public:
    explicit AdrenoSamplerA6XX(const AdrenoGPU& gpu, std::unique_ptr<Handle> handle);
    ~AdrenoSamplerA6XX() override = default;
};

} // namespace adreno
} // namespace backend
} // namespace hpc