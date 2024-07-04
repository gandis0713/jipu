#include "hpc/backend/instance.h"

#include "instance_impl.h"

namespace hpc
{
namespace backend
{

std::unique_ptr<Instance> Instance::create(const BackendType type)
{
    return InstanceImpl::create(type);
}

} // namespace backend
} // namespace hpc