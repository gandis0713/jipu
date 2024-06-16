#pragma once

#include "gpu.h"

#include <vector>

namespace hpc
{

std::vector<std::unique_ptr<GPU>> gpus();

} // namespace hpc