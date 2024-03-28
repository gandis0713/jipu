#include <device/product_id.hpp>
#include <hwcpipe/counter_database.hpp>
#include <hwcpipe/gpu.hpp>
#include <hwcpipe/sampler.hpp>

#include <map>

namespace jipu
{

namespace base
{

class MaliGPU
{
public:
    explicit MaliGPU(int deviceNumber);

public:
    const std::vector<hwcpipe_counter>& getCounters(hwcpipe::gpu gpu) const;
    hwcpipe::counter_sample getSample(hwcpipe_counter counter) const;

    std::error_code startSampling();
    std::error_code stopSampling();

private:
    hwcpipe::gpu m_gpu;
    std::vector<hwcpipe_counter> m_counters{};
    hwcpipe::sampler<> m_sampler;
};

class HWCPipe
{
public:
    HWCPipe();

public:
    const std::vector<MaliGPU>& getGpus();

private:
    std::vector<MaliGPU> m_gpus{};
};

} // namespace base

} // namespace jipu