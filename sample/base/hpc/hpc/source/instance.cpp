#include "hpc/instance.h"

#include "hpc/gpu.h"

#include "mali/mali_instance.h"

namespace hpc
{

std::unique_ptr<Instance> Instance::create(const GPUVendor vendor)
{
    switch (vendor)
    {
    case GPUVendor::Mali:
        return std::make_unique<mali::MaliInstance>();
    case GPUVendor::Adreno:
        return nullptr;
    default:
        return nullptr;
    }
}

} // namespace hpc