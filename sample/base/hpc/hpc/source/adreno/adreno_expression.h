#pragma once

#include "hpc/backend/sampler.h"
#include "hpc/counter.h"
#include "hpc/sampler.h"

namespace hpc
{
namespace adreno
{
namespace expression
{

using Samples = std::unordered_map<hpc::backend::Counter, hpc::backend::Sample>;

hpc::Sample nonFragmentUtilization(const hpc::Counter& counter, const Samples& samples);
hpc::Sample fragmentUtilization(const hpc::Counter& counter, const Samples& samples);

} // namespace expression
} // namespace adreno
} // namespace hpc