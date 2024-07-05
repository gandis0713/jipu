#include "hpc/backend/instance.h"

#include "device/adreno/adreno_instance.h"
#include "device/mali/mali_instance.h"

namespace hpc
{
namespace backend
{

std::unique_ptr<Instance> Instance::create(const DeviceType type)
{
    switch (type)
    {
    case DeviceType::Adreno:
        return adreno::AdrenoInstance::create();
    case DeviceType::Mali:
        return mali::MaliInstance::create();
    default:
        return nullptr;
    }
}

} // namespace backend
} // namespace hpc