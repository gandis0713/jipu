#include "mali_instance.h"

#include "mali_gpu.h"

#include <device/product_id.hpp>
#include <hwcpipe/counter_database.hpp>
#include <hwcpipe/gpu.hpp>
#include <hwcpipe/sampler.hpp>

#include <spdlog/spdlog.h>

namespace hpc
{
namespace mali
{

std::string productFamilyName(hwcpipe::device::gpu_family family)
{
    using gpu_family = hwcpipe::device::gpu_family;

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

std::vector<std::unique_ptr<GPU>> MaliInstance::gpus()
{
    std::vector<std::unique_ptr<GPU>> gpus{};
    for (const auto& gpu : hwcpipe::find_gpus())
    {
        spdlog::debug("------------------------------------------------------------");
        spdlog::debug("GPU Device {} :", gpu.get_device_number());
        spdlog::debug("------------------------------------------------------------");
        spdlog::debug("    Product Family:    {}", productFamilyName(gpu.get_gpu_family()));
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