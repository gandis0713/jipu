#include "adreno_gpu.h"

#include "adreno_instance.h"
#include "adreno_sampler.h"
#include "adreno_sampler_a6xx.h"

#include "ioctl/types.h"
#include "ioctl/utils.h"

#include <spdlog/spdlog.h>

namespace hpc
{
namespace backend
{
namespace adreno
{

AdrenoGPU::AdrenoGPU(AdrenoInstance& instance, const std::string& path)
    : m_instance(instance)
    , m_path(path)
{
}

Instance& AdrenoGPU::getInstance()
{
    return static_cast<Instance&>(m_instance);
}

std::unique_ptr<Sampler> AdrenoGPU::createSampler()
{
    auto handle = hpc::backend::Handle::create(m_path);
    spdlog::error("charles Failed to create adreno device handle");
    if (!handle)
    {
        spdlog::error("Failed to create adreno device handle");
        return nullptr;
    }

    auto series = getSeries(handle->fd());
    switch (series)
    {
    case AdrenoSeries::HPC_GPU_ADRENO_SERIES_A6XX:
        return std::make_unique<AdrenoSamplerA6XX>(*this, std::move(handle));
    default:
        return nullptr;
    }
}

} // namespace adreno
} // namespace backend
} // namespace hpc