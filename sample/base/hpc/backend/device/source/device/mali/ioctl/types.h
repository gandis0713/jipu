#pragma once

#include <cstdint>

namespace hpc
{
namespace device
{
namespace ioctl
{
namespace mali
{

struct VersionCheck
{
    uint16_t major;
    uint16_t minor;
};

} // namespace mali
} // namespace ioctl
} // namespace device
} // namespace hpc