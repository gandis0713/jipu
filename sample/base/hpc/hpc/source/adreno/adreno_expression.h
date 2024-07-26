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

using Key_t = hpc::backend::Counter;
using Value_t = hpc::backend::Sample;
using Samples = std::unordered_map<Key_t, Value_t>;

hpc::Sample nonFragmentUtilization(const hpc::Counter& counter, const Samples& samples);
hpc::Sample fragmentUtilization(const hpc::Counter& counter, const Samples& samples);
hpc::Sample convert(const hpc::Counter& counter, const Samples& samples);

} // namespace expression
} // namespace adreno
} // namespace hpc