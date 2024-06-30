#include "adreno_instance.h"

#include "adreno_gpu.h"

#include <spdlog/spdlog.h>

#include "device/handle.h"

namespace hpc
{
namespace adreno
{

std::vector<std::unique_ptr<GPU>> AdrenoInstance::gpus()
{
    auto handle = hpc::device::Handle::create("/dev/kgsl-3d0");
    if (!handle)
    {
        spdlog::error("Failed to create device handle");
        return {};
    }

    return {};
}

} // namespace adreno
} // namespace hpc