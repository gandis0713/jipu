#include "hpc/instance.h"

#include "hpc/gpu.h"

#if defined(__ANDROID__) || defined(ANDROID) || defined(__linux__)
#include "adreno/adreno_instance.h"
#include "mali/mali_instance.h"
#endif

namespace hpc
{

std::unique_ptr<Instance> Instance::create(const InstanceDescriptor& descriptor)
{
    switch (descriptor.gpuType)
    {
#if defined(__ANDROID__) || defined(ANDROID) || defined(__linux__)
    case GPUType::Mali:
        return std::make_unique<mali::MaliInstance>();
    case GPUType::Adreno:
        return std::make_unique<adreno::AdrenoInstance>();
#endif

    default:
        return nullptr;
    }
}

} // namespace hpc