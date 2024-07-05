#include "adreno_instance.h"

#include "adreno_gpu.h"
#include "hpc/backend/instance.h"

#include <spdlog/spdlog.h>

namespace hpc
{
namespace adreno
{

std::vector<std::unique_ptr<GPU>> AdrenoInstance::gpus()
{
    m_instance = hpc::backend::Instance::create(hpc::backend::DeviceType::Adreno);
    if (!m_instance)
    {
        spdlog::error("Failed to create device instance");
        return {};
    }

    return {};
}

} // namespace adreno
} // namespace hpc