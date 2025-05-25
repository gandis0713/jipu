#include "june_noapi_service1.h"

#include <random>
#include <spdlog/fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <thread>

#include "unique_handle.h"

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

    // Create Fence
    {
        std::string label = "noapi service1 fence";
        JuneFenceCreateDescriptor fenceDescriptor;
        fenceDescriptor.label.data = label.c_str();
        fenceDescriptor.label.length = static_cast<uint32_t>(label.length());
        m_signalFence = m_juneAPI.ApiContextCreateFence(m_juneApiContext, &fenceDescriptor);
    }
}

void JuneNoApiService1::work()
{
    std::vector<UniqueHandle> waitSyncFDs{};

    std::vector<JuneFence> waitFences = getWaitFences();

    spdlog::trace("Charles Try to get sync object in noapi service1.");
    for (const auto& fence : waitFences)
    {
        JuneFenceSyncFDExportDescriptor syncFDExportDescriptor{};
        syncFDExportDescriptor.chain.sType = JuneSType_FenceSyncFDExportDescriptor;
        syncFDExportDescriptor.syncFD = -1;

        JuneFenceExportDescriptor descriptor{};
        descriptor.nextInChain = &syncFDExportDescriptor.chain;
        descriptor.fence = fence;

        m_juneAPI.ApiContextExportFence(m_juneApiContext, &descriptor);

        if (syncFDExportDescriptor.syncFD != -1)
        {
            waitSyncFDs.push_back(UniqueHandle(syncFDExportDescriptor.syncFD));
        }
    }

#if defined(__ANDROID__) || defined(ANDROID)
    if (!waitSyncFDs.empty())
    {
        auto mergedHandle = UniqueHandle::merge("no api service1 fences", std::move(waitSyncFDs));
        int syncFD = mergedHandle.release(); // release for transfer ownership

        spdlog::debug("Charles merged sync fd: {}", syncFD);

        // Check buffer sync
        if (false)
        {
            // Gralloc takes ownership of the sync fd and closes it when it is done with it.
            // Ref: https://cgit.freedesktop.org/mesa/mesa/commit/?id=932f51d593418c95bf8f56ac9335d5f6c52c1285

            void* ptr;
            int result = AHardwareBuffer_lock(m_aHardwareBuffers[0], AHARDWAREBUFFER_USAGE_CPU_READ_OFTEN, syncFD, nullptr, &ptr);
            if (result != 0)
            {
                spdlog::error("Failed to AHardwareBuffer lock. {}", result);
            }

            // 4) 기준 색 추출 (첫 번째 픽셀)
            auto* base = static_cast<const uint8_t*>(ptr);
            std::vector<uint8_t> reference(base, base + m_bytesPerPixels[0]);

            bool isMonochrome = true;
            const uint32_t rowPitch = m_aHardwareBufferDescs[0].stride * m_bytesPerPixels[0];
            for (uint32_t y = 0; y < m_aHardwareBufferDescs[0].height && isMonochrome; ++y)
            {
                const uint8_t* row = base + y * rowPitch;
                for (uint32_t x = 0; x < m_aHardwareBufferDescs[0].width; ++x)
                {
                    const uint8_t* pixel = row + x * m_bytesPerPixels[0];
                    if (memcmp(pixel, reference.data(), m_bytesPerPixels[0]) != 0)
                    {
                        isMonochrome = false;
                        spdlog::warn("diff color = ({:3d}, {:3d}, {:3d}, {:3d})",
                                     pixel[0], pixel[1], pixel[2], pixel[3]);
                        break;
                    }
                }
            }

            spdlog::info("Base color RGBA8 = {:3d} {:3d} {:3d} {:3d}",
                         reference[0], reference[1], reference[2], reference[3]);

            if (isMonochrome)
            {
                spdlog::info("AHardwareBuffer monochrome check: uniform color");
            }
            else
            {
                spdlog::error("AHardwareBuffer monochrome check: mixed colors");
            }

            if (m_signalFD != -1)
            {
                close(m_signalFD);
                m_signalFD = -1;
            }

            result = AHardwareBuffer_unlock(m_aHardwareBuffers[0], &m_signalFD);
            if (result != 0)
            {
                spdlog::error("Failed to AHardwareBuffer unlock. {}", result);
            }
        }
        else
        {
            close(syncFD);
        }
    }

#endif

    spdlog::debug("no api service1 end access");

    if (m_signalFD != -1)
    {
        JuneFenceSyncFDResetDescriptor syncFDResetDescriptor{};
        syncFDResetDescriptor.chain.sType = JuneSType_FenceSyncFDResetDescriptor;
        syncFDResetDescriptor.syncFD = m_signalFD;

        JuneFenceResetDescriptor descriptor{};
        descriptor.nextInChain = &syncFDResetDescriptor.chain;

        m_juneAPI.FenceReset(m_signalFence, &descriptor);
    }
}
#if defined(__ANDROID__) || defined(ANDROID)
void JuneNoApiService1::addAHardwareBuffer(AHardwareBuffer* aHardwareBuffer)
{
    if (aHardwareBuffer)
    {
        AHardwareBuffer_Desc aHardwareBufferDesc;
        AHardwareBuffer_describe(aHardwareBuffer, &aHardwareBufferDesc);

        uint32_t bytesPerPixel = 0;
        switch (aHardwareBufferDesc.format)
        {
        case AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM:
        case AHARDWAREBUFFER_FORMAT_R8G8B8X8_UNORM:
            bytesPerPixel = 4;
            break;
        case AHARDWAREBUFFER_FORMAT_R5G6B5_UNORM:
            bytesPerPixel = 2;
            break;
        default:
            spdlog::warn("Unsupported buffer format (0x{:X});", aHardwareBufferDesc.format);
            return;
        }

        m_aHardwareBuffers.push_back(aHardwareBuffer);
        m_aHardwareBufferDescs.push_back(aHardwareBufferDesc);
        m_bytesPerPixels.push_back(bytesPerPixel);
    }
}
#endif

} // namespace jipu
