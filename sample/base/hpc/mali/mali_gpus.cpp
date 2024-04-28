#include "mali_gpus.h"

#include "mali_gpu.h"

#include <hwcpipe/gpu.hpp>
#include <spdlog/spdlog.h>

namespace jipu
{
namespace hpc
{
namespace mali
{

std::string getProductFamilyName(hwcpipe::device::product_id::gpu_family family)
{
    using gpu_family = hwcpipe::device::product_id::gpu_family;

    switch (family)
    {
    case gpu_family::bifrost:
        return "Bifrost";
    case gpu_family::midgard:
        return "Midgard";
    case gpu_family::valhall:
        return "Valhall";
    default:
        return "Unknown";
    }
}
std::vector<GPU::Ptr> gpus()
{
    std::vector<GPU::Ptr> gpus{};
    for (const auto& gpu : hwcpipe::find_gpus())
    {
        spdlog::debug("------------------------------------------------------------");
        spdlog::debug("GPU Device {} :", gpu.get_device_number());
        spdlog::debug("------------------------------------------------------------");
        spdlog::debug("    Product Family:    {}", getProductFamilyName(gpu.get_product_id().get_gpu_family()));
        spdlog::debug("    Number of Cores:   {}", gpu.num_shader_cores());
        spdlog::debug("    Number of Engines: {}", gpu.num_execution_engines());
        spdlog::debug("    Bus Width:         {}", gpu.bus_width());

        auto maliGPU = std::make_unique<MaliGPU>(gpu.get_device_number());
        gpus.push_back(std::move(maliGPU));
    }

    return gpus;
}

} // namespace mali
} // namespace hpc
} // namespace jipu