#include "adreno_instance.h"

#include "adreno_gpu.h"
#include "hpc/backend/instance.h"

#include <spdlog/spdlog.h>

namespace hpc
{
namespace adreno
{

std::unique_ptr<hpc::Instance> AdrenoInstance::create()
{
    auto instance = hpc::backend::Instance::create(hpc::backend::DeviceType::Adreno);
    if (!instance)
    {
        spdlog::error("Failed to create device instance");
        return nullptr;
    }

    return std::make_unique<AdrenoInstance>(std::move(instance));
}

AdrenoInstance::AdrenoInstance(std::unique_ptr<hpc::backend::Instance> instance)
    : m_instance(std::move(instance))
{
}

std::vector<std::unique_ptr<hpc::GPU>> AdrenoInstance::gpus()
{
    auto backendGPUs = m_instance->gpus();
    if (backendGPUs.empty())
    {
        spdlog::error("No GPUs found in backend.");
        return {};
    }

    std::vector<std::unique_ptr<hpc::GPU>> gpus{};
    for (auto& backendGPU : backendGPUs)
    {
        gpus.push_back(std::make_unique<hpc::adreno::AdrenoGPU>(*this, std::move(backendGPU)));
    }

    return gpus;
}

} // namespace adreno
} // namespace hpc