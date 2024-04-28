#pragma once

#include "sampler.h"

#include "mali_counter.h"

namespace jipu
{
namespace hpc
{
namespace mali
{

class MaliSampler final : public Sampler
{
public:
    explicit MaliSampler(MaliCounter counter);

private:
    const MaliCounter m_counter;
};

} // namespace mali
} // namespace hpc
} // namespace jipu