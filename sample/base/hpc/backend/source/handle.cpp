#include "hpc/backend/handle.h"

#include "handle_impl.h"

namespace hpc
{
namespace device
{

std::unique_ptr<Handle> Handle::create(const char* path)
{
    return HandleImpl::create(path);
}

} // namespace device
} // namespace hpc