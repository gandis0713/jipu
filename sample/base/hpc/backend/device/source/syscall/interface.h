#pragma once

#if defined(_WIN32)
#include "api/windows.h"
#else
#include "api/unix.h"
#endif

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

template <class T>
class Interface
{
private:
    using SyscallAPI_T = T;

public:
    static std::pair<std::error_code, int> open(const char* name, int oflags)
    {
        const int fd = SyscallAPI_T::open(name, oflags);

        std::error_code ec;
        if (fd < 0)
            ec = errorCode();

        return std::make_pair(ec, fd);
    }

    static std::error_code close(int fd)
    {
        const int result = SyscallAPI_T::close(fd);

        std::error_code ec;
        if (result < 0)
            ec = errorCode();

        return ec;
    }

    template <typename command_t, typename... args_t>
    static std::pair<std::error_code, int> ioctl(int fd, command_t command, args_t&&... args)
    {
        const int result = SyscallAPI_T::ioctl(fd, command, std::forward<args_t>(args)...);

        std::error_code ec;
        if (result < 0)
            ec = errorCode();

        return std::make_pair(ec, result);
    }

    static std::pair<std::error_code, int> poll(struct pollfd* fds, nfds_t nfds, int timeout)
    {
        const int result = SyscallAPI_T::poll(fds, nfds, timeout);

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

#if defined(WIN32)
using Interface = impl::Interface<api::Windows>;
#else
using Interface = impl::Interface<api::Unix>;
#endif

} // namespace syscall
} // namespace device
} // namespace hpc