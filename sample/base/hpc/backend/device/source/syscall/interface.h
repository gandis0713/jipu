#pragma once

#include <errno.h>
#include <sys/stat.h>
#include <system_error>

#include <utility>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

namespace hpc
{
namespace device
{
namespace syscall
{

// template <class T>
class Interface
{
private:
    // using SyscallAPI_Type = T;

public:
    static std::pair<std::error_code, int> open(const char* name, int oflags)
    {
        const int result = ::open(name, oflags);
        std::error_code ec;

        if (result < 0)
            ec = { errno, std::generic_category() };

        return std::make_pair(ec, result);
    }
};

} // namespace syscall
} // namespace device
} // namespace hpc