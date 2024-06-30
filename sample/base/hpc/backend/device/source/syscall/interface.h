#pragma once

#include "api/unix.h"

#include <system_error>
#include <utility>

namespace hpc
{
namespace device
{
namespace syscall
{

namespace impl
{
template <class SyscallAPI_T>
class Interface
{
private:
    using SyscallAPI = SyscallAPI_T;

public:
    static std::pair<std::error_code, int> open(const char* name, int oflags)
    {
        const int fd = SyscallAPI::open(name, oflags);

        std::error_code ec;
        if (fd < 0)
            ec = errorCode();

        return std::make_pair(ec, fd);
    }

    static std::error_code close(int fd)
    {
        const int result = SyscallAPI::close(fd);

        std::error_code ec;
        if (result < 0)
            ec = errorCode();

        return ec;
    }

    template <typename Command, typename... Args>
    static std::pair<std::error_code, int> ioctl(int fd, Command command, Args&&... args)
    {
        const int result = SyscallAPI::ioctl(fd, command, std::forward<Args>(args)...);

        std::error_code ec;
        if (result < 0)
            ec = errorCode();

        return std::make_pair(ec, result);
    }

    static std::pair<std::error_code, int> poll(struct pollfd* fds, nfds_t nfds, int timeout)
    {
        const int result = SyscallAPI::poll(fds, nfds, timeout);

        std::error_code ec;

        if (result < 0)
            ec = errorCode();

        return std::make_pair(ec, result);
    }

private:
    static std::error_code errorCode()
    {
        return { errno, std::generic_category() };
    }
};
} // namespace impl

using Interface = impl::Interface<api::Unix>;

} // namespace syscall
} // namespace device
} // namespace hpc