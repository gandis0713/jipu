#include "hpc/backend/instance.h"

#include "instance_impl.h"

namespace hpc
{
namespace backend
{

std::unique_ptr<Instance> Instance::create(std::unique_ptr<Handle> handle)
{
    return std::make_unique<InstanceImpl>(std::move(handle));
}

} // namespace backend
} // namespace hpc