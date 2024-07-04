#include "instance_impl.h"

#include "handle_impl.h"
#include "device/adreno/ioctl/a6xx.h"
#include "device/adreno/ioctl/types.h"
#include "device/adreno/ioctl/utils.h"
#include "syscall/interface.h"

#include <spdlog/spdlog.h>
#include <thread>

namespace hpc
{
namespace device
{

InstanceImpl::InstanceImpl(std::unique_ptr<Handle> handle)
    : m_handle(std::move(handle))
{
    init();
}

void InstanceImpl::init()
{
    auto t = std::thread([&]() {
        adreno_device_info devinfo{};

        adreno_device_get_property deviceGetProperty{};
        deviceGetProperty.type = ADRENO_PROPERTY_DEVICE_INFO;
        deviceGetProperty.value = &devinfo;
        deviceGetProperty.num_bytes = sizeof(adreno_device_info);

        auto& handleImpl = downcast(*m_handle);
        auto result = syscall::Interface::ioctl(handleImpl.fd(), ADRENO_IOCTL_DEVICE_GET_PROPERTY, &deviceGetProperty);
        auto error = result.first;
        if (error)
        {
            // TODO: log
            return;
        }

        uint32_t chip_id = devinfo.chip_id;
        uint8_t core_id = (chip_id >> (8 * 3)) & 0xffu;
        uint8_t major_id = (chip_id >> (8 * 2)) & 0xffu;
        uint8_t minor_id = (chip_id >> (8 * 1)) & 0xffu;
        auto gpu_id = core_id * 100 + major_id * 10 + minor_id;

        spdlog::debug("charles chip_id: {}", chip_id);
        spdlog::debug("charles devinfo.device_id: {}", devinfo.device_id);
        spdlog::debug("charles gpu_id: {}", gpu_id);
        spdlog::debug("charles error.code: {}", error.value());
        spdlog::debug("charles error.message: {}", error.message());

        auto series = getSeries(gpu_id);

        spdlog::debug("charles series: {}", static_cast<uint32_t>(series));

        switch (series)
        {
        case AdrenoSeries::HPC_GPU_ADRENO_SERIES_A6XX: {

            std::vector<adreno_a6xx_counter> counters = {
                adreno_a6xx_counter::HPC_GPU_ADRENO_A6XX_SP_BUSY_CYCLES,
                adreno_a6xx_counter::HPC_GPU_ADRENO_A6XX_SP_VS_INSTRUCTIONS,
                adreno_a6xx_counter::HPC_GPU_ADRENO_A6XX_SP_FS_INSTRUCTIONS,
                adreno_a6xx_counter::HPC_GPU_ADRENO_A6XX_SP_CS_INSTRUCTIONS,
                adreno_a6xx_counter::HPC_GPU_ADRENO_A6XX_SP_GPR_READ_CONFLICT,  // A6XX specific
                adreno_a6xx_counter::HPC_GPU_ADRENO_A6XX_SP_GPR_WRITE_CONFLICT, // A6XX specific
            };

            for (int i = 0; i < counters.size(); ++i)
            {
                auto groupId = getGroup(counters[i]);
                auto countableSelector = getSelector(counters[i]);

                adreno_counter_get counterGet{};
                counterGet.group_id = groupId;
                counterGet.countable_selector = countableSelector;
                result = syscall::Interface::ioctl(handleImpl.fd(), ADRENO_IOCTL_COUNTER_GET, &counterGet);

                spdlog::debug("charles groupid {}", groupId);
                spdlog::debug("charles countableSelector {}", countableSelector);
                spdlog::debug("charles error.code {}", result.first.value());
                spdlog::debug("charles error.message {}", result.first.message());
                spdlog::debug("charles result {}", result.second);
                if (result.first)
                {
                    return;
                }
            }

            auto groupId = getGroup(counters[0]);
            adreno_perfcounter_query counterQuery{};
            counterQuery.groupid = groupId;
            result = syscall::Interface::ioctl(handleImpl.fd(), ADRENO_IOCTL_PERFCOUNTER_QUERY, &counterQuery);

            if (result.first)
            {
                spdlog::debug("charles error.code {}", result.first.value());
                spdlog::debug("charles error.message {}", result.first.message());
                spdlog::debug("charles result {}", result.second);
            }

            spdlog::debug("charles counterQuery.groupid {}", counterQuery.groupid);
            spdlog::debug("charles counterQuery.count {}", counterQuery.count);
            spdlog::debug("charles counterQuery.max_counters {}", counterQuery.max_counters);

            for (auto i = 0; i < counterQuery.count; ++i)
            {
                spdlog::debug("charles counterQuery.countables[{}] {}", i, counterQuery.countables[i]);
            }

            while (true)
            {
                std::vector<hpc_gpu_adreno_ioctl_counter_read_counter_t> counterReadValues{};
                for (int i = 0; i < counters.size(); ++i)
                {
                    hpc_gpu_adreno_ioctl_counter_read_counter_t counterReadValue{};
                    counterReadValue.group_id = getGroup(counters[i]);
                    counterReadValue.countable_selector = getSelector(counters[i]);

                    counterReadValues.push_back(counterReadValue);
                }

                adreno_counter_read counterRead{};
                counterRead.num_counters = static_cast<unsigned int>(counterReadValues.size());
                counterRead.counters = reinterpret_cast<hpc_gpu_adreno_ioctl_counter_read_counter_t*>(counterReadValues.data());

                result = syscall::Interface::ioctl(handleImpl.fd(), ADRENO_IOCTL_COUNTER_READ, &counterRead);

                spdlog::debug("charles error.code {}", result.first.value());
                spdlog::debug("charles error.message {}", result.first.message());
                spdlog::debug("charles result {}", result.second);

                std::this_thread::sleep_for(std::chrono::seconds(1));
                if (result.first)
                {
                    // TODO: log error
                    continue;
                }

                for (int i = 0; i < counterReadValues.size(); ++i)
                {
                    spdlog::debug("charles counter {} value {}", static_cast<uint64_t>(counters[i]), counterReadValues[i].value);
                }
            }
        }
        break;
        case AdrenoSeries::HPC_GPU_ADRENO_SERIES_A5XX:

            break;
        default:
            // do nothing
            break;
        }
    });
    t.detach();
}

} // namespace device
} // namespace hpc