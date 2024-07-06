#include "adreno_gpu.h"

#include "adreno_instance.h"
#include "adreno_sampler.h"

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
    if (!handle)
    {
        spdlog::error("Failed to create adreno device handle");
        return nullptr;
    }

    return std::make_unique<AdrenoSampler>(*this, std::move(handle));
}

} // namespace adreno
} // namespace backend
} // namespace hpc