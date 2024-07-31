#include "adreno_gpu.h"

#include "adreno_instance.h"
#include "adreno_sampler.h"
#include "adreno_sampler_a6xx.h"

#include "ioctl/api.h"
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

std::unique_ptr<Sampler> AdrenoGPU::createSampler(const SamplerDescriptor& descriptor)
{
    auto handle = hpc::backend::Handle::create(m_path);
    if (!handle)
    {
        spdlog::error("Failed to create adreno device handle");
        return nullptr;
    }

    auto series = getSeries(handle->fd());
    switch (series)
    {
    case AdrenoSeries::A6XX:
        return std::make_unique<AdrenoSamplerA6XX>(*this, std::move(handle), descriptor);
    default:
        spdlog::error("Failed to create adreno sampler");
        return nullptr;
    }
}

} // namespace adreno
} // namespace backend
} // namespace hpc