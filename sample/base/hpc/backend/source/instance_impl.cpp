#include "instance_impl.h"

#include "device/adreno/adreno_instance_impl.h"
#include "device/mali/mali_instance_impl.h"

namespace hpc
{
namespace backend
{

std::unique_ptr<Instance> InstanceImpl::create(const BackendType type)
{
    switch (type)
    {
    case BackendType::Adreno:
        return AdrenoInstanceImpl::create();
    case BackendType::Mali:
        return MaliInstanceImpl::create();
    default:
        return nullptr;
    }
}

} // namespace backend
} // namespace hpc