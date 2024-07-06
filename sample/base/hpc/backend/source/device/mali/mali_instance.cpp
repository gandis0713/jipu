#include "mali_instance.h"
// #include "mali_gpu.h"

#include <spdlog/spdlog.h>

namespace hpc
{
namespace backend
{
namespace mali
{

std::vector<std::unique_ptr<hpc::backend::GPU>> MaliInstance::gpus()
{
    static std::vector<const char*> paths = {
        "/dev/mali0",
    };

    // TODO

    return {};
}

} // namespace mali
} // namespace backend
} // namespace hpc