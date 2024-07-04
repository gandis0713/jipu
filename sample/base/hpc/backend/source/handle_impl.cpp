#include "handle_impl.h"

#include "syscall/interface.h"

#include <linux/ioctl.h>

namespace hpc
{
namespace backend
{

std::unique_ptr<Handle> HandleImpl::create(const char* path)
{
    auto ret = syscall::Interface::open(path, O_RDONLY /* O_RDONLY */);
    auto error = ret.first;
    if (error)
    {
        // TODO: Log
        return nullptr;
    }

    auto fd = ret.second;
    return std::make_unique<HandleImpl>(fd);
}

HandleImpl::HandleImpl(const int fd)
    : m_fd(fd)
{
}

HandleImpl::~HandleImpl()
{
    auto error = syscall::Interface::close(m_fd);
    if (error)
    {
        // TODO: Log
    }
}

int HandleImpl::fd()
{
    return m_fd;
}

} // namespace backend
} // namespace hpc