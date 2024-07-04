#include "mali_instance_impl.h"

#include "hpc/backend/handle.h"

#include <spdlog/spdlog.h>

namespace hpc
{
namespace backend
{

std::unique_ptr<Instance> MaliInstanceImpl::create()
{
    auto handle = hpc::backend::Handle::create("/dev/mali0");
    if (!handle)
    {
        spdlog::error("Failed to create mali device handle");
        return nullptr;
    }

    return std::make_unique<MaliInstanceImpl>(std::move(handle));
}

MaliInstanceImpl::MaliInstanceImpl(std::unique_ptr<Handle> handle)
    : m_handle(std::move(handle))
{
}

} // namespace backend
} // namespace hpc