#include "hpc/backend/handle.h"

#include "handle_impl.h"

namespace hpc
{
namespace backend
{

std::unique_ptr<Handle> Handle::create(const char* path)
{
    return HandleImpl::create(path);
}

} // namespace backend
} // namespace hpc