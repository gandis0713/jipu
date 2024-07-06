#include "adreno_instance.h"
#include "adreno_gpu.h"

#include "ioctl/a6xx.h"
#include "ioctl/types.h"
#include "ioctl/utils.h"
#include "syscall/interface.h"

#include <spdlog/spdlog.h>
#include <thread>

namespace hpc
{
namespace backend
{
namespace adreno
{

std::vector<std::unique_ptr<hpc::backend::GPU>> AdrenoInstance::gpus()
{
    static std::vector<const char*> paths = {
        "/dev/kgsl-3d0",
    };

    std::vector<std::unique_ptr<GPU>> gpus{};
    for (const auto& path : paths)
    {
        gpus.push_back(std::make_unique<AdrenoGPU>(*this, path));
    }

    return gpus;
}

} // namespace adreno
} // namespace backend
} // namespace hpc