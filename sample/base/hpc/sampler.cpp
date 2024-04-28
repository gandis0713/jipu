#include "sampler.h"

namespace jipu
{
namespace hpc
{

Sampler::Sampler(GPU::Ptr gpu)
    : m_gpu(std::move(gpu))
{
}

} // namespace hpc
} // namespace jipu