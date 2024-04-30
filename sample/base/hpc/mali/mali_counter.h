#pragma once

#include "counter.h"

#include <hwcpipe/hwcpipe_counter.h>

namespace jipu
{
namespace hpc
{
namespace mali
{

Counter hwcToCounter(hwcpipe_counter counter);
hwcpipe_counter counterToHwc(Counter counter);

} // namespace mali
} // namespace hpc
} // namespace jipu