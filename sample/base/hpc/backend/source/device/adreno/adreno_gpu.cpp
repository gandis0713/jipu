#include "adreno_gpu.h"

#include "adreno_sampler.h"

namespace hpc
{
namespace backend
{
namespace adreno
{

AdrenoGPU::AdrenoGPU(const std::string& path)
    : m_path(path)
{
}

std::unique_ptr<Sampler> AdrenoGPU::createSampler()
{
    return std::make_unique<AdrenoSampler>(*this);
}

} // namespace adreno
} // namespace backend
} // namespace hpc