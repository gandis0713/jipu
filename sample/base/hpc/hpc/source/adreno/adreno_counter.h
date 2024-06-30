#pragma once

#include "hpc/counter.h"

#include <hwcpipe/hwcpipe_counter.h>
#include <unordered_map>
#include <vector>

namespace hpc
{
namespace adreno
{

// extern const std::unordered_map<Counter, std::vector<hwcpipe_counter>> counterDependencies;
const std::unordered_map<Counter, std::vector<hwcpipe_counter>> counterDependencies{};

} // namespace adreno
} // namespace hpc