#include "hpc/instance.h"

#include "hpc/gpu.h"

#if defined(__ANDROID__) || defined(ANDROID) || defined(__linux__)
#include "mali/mali_instance.h"
#endif

namespace hpc
{

std::unique_ptr<Instance> Instance::create(const GPUVendor vendor)
{
    switch (vendor)
    {
#if defined(__ANDROID__) || defined(ANDROID) || defined(__linux__)
    case GPUVendor::Mali:
        return std::make_unique<mali::MaliInstance>();
#endif
    case GPUVendor::Adreno:
        return nullptr;
    default:
        return nullptr;
    }
}

} // namespace hpc