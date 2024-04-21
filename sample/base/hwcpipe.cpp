#include "hwcpipe.h"

#include <spdlog/spdlog.h>

namespace jipu
{

namespace
{

std::string getProductFamilyName(hwcpipe::device::product_id::gpu_family family)
{
    using gpu_family = hwcpipe::device::product_id::gpu_family;

    switch (family)
    {
    case gpu_family::bifrost:
        return "Bifrost";
    case gpu_family::midgard:
        return "Midgard";
    case gpu_family::valhall:
        return "Valhall";
    default:
        return "Unknown";
    }
}

void printSampleValue(const hwcpipe::counter_sample& sample)
{
    switch (sample.type)
    {
    case hwcpipe::counter_sample::type::uint64:
        spdlog::debug(sample.value.uint64);
        return;
    case hwcpipe::counter_sample::type::float64:
        spdlog::debug(sample.value.float64);
        return;
    }
}

} // namespace

MaliGPU::MaliGPU(int deviceNumber)
    : m_gpu(deviceNumber) // Probe device 0 (i.e. /dev/mali0)
    , m_config(hwcpipe::sampler_config(m_gpu))
    , m_sampler(m_config) // empty counter sampler
{
    gatherCounters();
}

MaliGPU::~MaliGPU()
{
}

hwcpipe::device::constants MaliGPU::getInfo() const
{
    return m_gpu.get_constants();
}

const std::unordered_set<hwcpipe_counter>& MaliGPU::getCounters() const
{
    return m_counters;
}

hwcpipe::counter_sample MaliGPU::getSample(hwcpipe_counter counter) const
{
    hwcpipe::counter_sample sample;

    std::error_code ec = m_sampler.get_counter_value(counter, sample);
    if (ec)
        spdlog::error("Failed to get counter value. couinter: {}, error: {}", static_cast<uint32_t>(counter), ec.message());

    return sample;
}

std::error_code MaliGPU::startSampling()
{
    std::error_code ec = m_sampler.start_sampling();

    if (ec)
        spdlog::error("Failed to start sampling. error: {}", ec.message());

    return ec;
}

std::error_code MaliGPU::stopSampling()
{
    std::error_code ec = m_sampler.stop_sampling();

    if (ec)
        spdlog::error("Failed to stop sampling. error: {}", ec.message());

    return ec;
}

void MaliGPU::gatherCounters()
{
    spdlog::debug("------------------------------------------------------------");
    spdlog::debug("GPU {} Supported counters:", m_gpu.get_device_number());
    spdlog::debug("------------------------------------------------------------");

    hwcpipe::counter_metadata meta;
    hwcpipe::counter_database counterDB{};
    for (hwcpipe_counter counter : counterDB.counters_for_gpu(m_gpu))
    {
        auto ec = counterDB.describe_counter(counter, meta);
        if (ec)
        {
            spdlog::error("counter error {}", ec.message());
            // Should not happen because counters_for_gpu(gpu) only returns
            // known counters, by definition
            assert(false);
        }

        spdlog::debug("    {}", meta.name);

        m_counters.emplace(counter);
    }
}

void MaliGPU::configureSampler(const std::unordered_set<hwcpipe_counter> counters)
{
    m_config = hwcpipe::sampler_config(m_gpu);

    std::error_code ec;
    for (const auto& counter : counters)
    {
        if (m_counters.contains(counter))
        {
            ec = m_config.add_counter(counter);
            if (ec)
                spdlog::error("GPU {} counter not supported by this GPU.", static_cast<uint32_t>(counter));
        }
    }

    m_sampler = hwcpipe::sampler<>(m_config);
}

HWCPipe::HWCPipe()
{
    for (const auto& gpu : hwcpipe::find_gpus())
    {
        spdlog::debug("------------------------------------------------------------");
        spdlog::debug("GPU Device {} :", gpu.get_device_number());
        spdlog::debug("------------------------------------------------------------");
        spdlog::debug("    Product Family:    {}", getProductFamilyName(gpu.get_product_id().get_gpu_family()));
        spdlog::debug("    Number of Cores:   {}", gpu.num_shader_cores());
        spdlog::debug("    Number of Engines: {}", gpu.num_execution_engines());
        spdlog::debug("    Bus Width:         {}", gpu.bus_width());

        m_gpus.emplace_back(gpu.get_device_number());
    }
}

std::vector<MaliGPU>& HWCPipe::getGpus()
{
    return m_gpus;
}

} // namespace jipu