#if defined(__ANDROID__) || defined(ANDROID)

#define HWC_PIPE_ENABLED 1
#include <device/product_id.hpp>
#include <hwcpipe/counter_database.hpp>
#include <hwcpipe/gpu.hpp>
#include <hwcpipe/sampler.hpp>

#include <map>

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
    const std::vector<hwcpipe_counter>& getCounters() const;
    hwcpipe::counter_sample getSample(hwcpipe_counter counter) const;

    std::error_code startSampling();
    std::error_code stopSampling();

private:
    void gatherCounters();
    void configureSampler();

private:
    hwcpipe::gpu m_gpu;
    std::vector<hwcpipe_counter> m_counters{};
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

#endif