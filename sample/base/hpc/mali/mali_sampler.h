#pragma once

#include "sampler.h"

namespace jipu
{
namespace hpc
{

class MaliSampler : public Sampler
{
public:
    explicit MaliSampler(Counter counter);
};

} // namespace hpc
} // namespace jipu