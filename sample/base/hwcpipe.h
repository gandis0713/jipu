#define HWC_PIPE_ENABLED 1
#include <device/product_id.hpp>
#include <hwcpipe/counter_database.hpp>
#include <hwcpipe/gpu.hpp>
#include <hwcpipe/sampler.hpp>

#include <unordered_set>

namespace jipu
{

class MaliGPU
{
public:
    MaliGPU(int deviceNumber);
    ~MaliGPU();

    MaliGPU(const MaliGPU&) = delete;
    MaliGPU& operator=(const MaliGPU&) = delete;

    MaliGPU(MaliGPU&&) = default;
    MaliGPU& operator=(MaliGPU&&) = default;

public:
    hwcpipe::device::constants getInfo() const;
    const std::unordered_set<hwcpipe_counter>& getCounters() const;
    void configureSampler(const std::unordered_set<hwcpipe_counter> counters);
    hwcpipe::counter_sample getSample(hwcpipe_counter counter) const;

    std::error_code startSampling();
    std::error_code stopSampling();

private:
    void gatherCounters();

private:
    hwcpipe::gpu m_gpu;
    std::unordered_set<hwcpipe_counter> m_counters{};
    hwcpipe::sampler_config m_config;
    hwcpipe::sampler<> m_sampler;

public:
    using Ref = std::reference_wrapper<MaliGPU>;
};

class HWCPipe
{
public:
    HWCPipe();

public:
    std::vector<MaliGPU>& getGpus();

private:
    std::vector<MaliGPU> m_gpus{};
};

} // namespace jipu