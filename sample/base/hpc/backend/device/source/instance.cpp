#include "device/instance.h"

#include "instance_impl.h"

namespace hpc
{
namespace device
{

std::unique_ptr<Instance> Instance::create(Handle& handle)
{
    return std::make_unique<InstanceImpl>(handle);
}

} // namespace device
} // namespace hpc