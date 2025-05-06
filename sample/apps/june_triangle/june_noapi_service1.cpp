#include "june_noapi_service1.h"

#include <random>
#include <spdlog/spdlog.h>
#include <thread>

namespace jipu
{

namespace
{

} // namespace

JuneNoApiService1::JuneNoApiService1(const JuneServiceDescriptor& descriptor)
    : JuneNoApiService(descriptor)
{
}

JuneNoApiService1::~JuneNoApiService1()
{
}

void JuneNoApiService1::begin()
{
    JuneNoApiService::begin();

    std::string label = "noapi service1";
    createInstance(label);
    createApiContext(label);

    // Create Shared Memory
    {
#if defined(__ANDROID__) || defined(ANDROID)
        AHardwareBuffer_Desc ahbDesc = {
            .width = m_descriptor.width,
            .height = m_descriptor.height,
            .layers = 1,
            .format = AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM,
            .usage = AHARDWAREBUFFER_USAGE_GPU_SAMPLED_IMAGE | AHARDWAREBUFFER_USAGE_GPU_COLOR_OUTPUT
        };

        int result = AHardwareBuffer_allocate(&ahbDesc, &m_aHardwareBuffer);
        if (result != 0)
        {
            spdlog::error("Failed to allocate AHardwareBuffer: {}", result);
            return;
        }

        JuneSharedMemoryAHardwareBufferImportDescriptor juneSharedMemoryAHardwareBufferImportDescriptor{};
        juneSharedMemoryAHardwareBufferImportDescriptor.chain.sType = JuneSType_SharedMemoryAHardwareBufferImportDescriptor;
        juneSharedMemoryAHardwareBufferImportDescriptor.aHardwareBuffer = m_aHardwareBuffer;

        JuneSharedMemoryImportDescriptor juneSharedMemoryDescriptor{};
        juneSharedMemoryDescriptor.nextInChain = &juneSharedMemoryAHardwareBufferImportDescriptor.chain;
        m_sharingMemory = m_juneAPI.InstanceImportSharedMemory(m_juneInstance, &juneSharedMemoryDescriptor);
#endif
    }

    // Create Fence
    {
        JuneFenceCreateDescriptor fenceDescriptor;
        m_signalFence = m_juneAPI.ApiContextCreateFence(m_juneApiContext, &fenceDescriptor);
    }

    // Export AhardwareBuffer from Shared Memory
    {
    }
}

void JuneNoApiService1::work()
{
    {
        std::vector<int> waitSyncFDs{};
        std::vector<JuneFence> waitFences = getWaitFences();
        for (const auto& fence : waitFences)
        {
            JuneFenceSyncFDExportDescriptor syncFDExportDescriptor{};
            syncFDExportDescriptor.chain.sType = JuneSType_FenceSyncFDExportDescriptor;

            JuneFenceExportDescriptor descriptor{};
            descriptor.nextInChain = &syncFDExportDescriptor.chain;

            m_juneAPI.FenceExport(fence, &descriptor);

            if (syncFDExportDescriptor.syncFD != -1)
            {
                waitSyncFDs.push_back(syncFDExportDescriptor.syncFD);
            }
        }
    }

    spdlog::debug("no api service1 begin access");

    {
    }

    spdlog::debug("no api service1 end access");

    {
        JuneFenceResetDescriptor descriptor{};
        m_juneAPI.FenceReset(m_signalFence, &descriptor);
    }
}

} // namespace jipu
