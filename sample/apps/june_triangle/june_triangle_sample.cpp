#include "june_triangle_sample.h"

#include "june_gles_service1.h"
#include "june_gles_service2.h"
#include "june_noapi_service1.h"
#include "june_vulkan_service1.h"
#include "june_vulkan_service2.h"
#include "june_vulkan_service3.h"

#include <spdlog/spdlog.h>

namespace jipu
{

JuneTriangleSample::JuneTriangleSample(const JuneSampleDescriptor& descriptor)
    : JuneSample(descriptor)
{
}

JuneTriangleSample::~JuneTriangleSample()
{
}

void JuneTriangleSample::init()
{
    JuneSample::init();

    loadJuneLibrary();
    createInstance("JuneTriangleSample");
    createSharedMemories();

    if (m_sharedMemories.empty())
    {
        spdlog::error("No shared memories created, exiting.");
        return;
    }

    uint32_t serviceCase = 1;
    if (serviceCase == 0)
    {
        {
            m_noapiService1 = std::make_unique<JuneNoApiService1>(JuneServiceDescriptor{ .fps = 30,
                                                                                         .width = m_width,
                                                                                         .height = m_height,
                                                                                         .windowHandle = nullptr,
                                                                                         .appPath = m_appPath,
                                                                                         .appDir = m_appDir,
                                                                                         .appHandle = m_handle });
            m_noapiService1->start(JuneServiceStartDescriptor{
                .callback = [this]() {
                    m_noapiService1Ready = true;
                } });
        }

        {
            m_glesService1 = std::make_unique<JuneGLESService1>(JuneServiceDescriptor{ .fps = 120,
                                                                                       .width = m_width,
                                                                                       .height = m_height,
                                                                                       .windowHandle = nullptr,
                                                                                       .appPath = m_appPath,
                                                                                       .appDir = m_appDir,
                                                                                       .appHandle = m_handle });
            m_glesService1->start(JuneServiceStartDescriptor{
                .callback = [this]() {
                    m_glesService1Ready = true;
                } });
        }

        {
            m_glesService2 = std::make_unique<JuneGLESService2>(JuneServiceDescriptor{ .fps = 30,
                                                                                       .width = m_width,
                                                                                       .height = m_height,
                                                                                       .windowHandle = getWindowHandle(),
                                                                                       .appPath = m_appPath,
                                                                                       .appDir = m_appDir,
                                                                                       .appHandle = m_handle });
            m_glesService2->start(JuneServiceStartDescriptor{
                .callback = [this]() {
                    m_glesService2Ready = true;
                } });
        }

        while (!m_noapiService1Ready ||
               !m_glesService1Ready ||
               !m_glesService2Ready)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

#if defined(__ANDROID__) || defined(ANDROID)
        for (auto aHardwareBuffer : m_aHardwareBuffers)
        {
            static_cast<JuneNoApiService1*>(m_noapiService1.get())->addAHardwareBuffer(aHardwareBuffer);
        }
#endif

        m_glesService1->addSharedMemory(m_sharedMemories[0]);
        m_glesService2->addSharedMemory(m_sharedMemories[0]);
        m_noapiService1->addSharedMemory(m_sharedMemories[0]);

        m_glesService1->connectMemoryNode(m_noapiService1->getMemoryNode());
        m_glesService2->connectMemoryNode(m_glesService1->getMemoryNode());
        m_noapiService1->connectMemoryNode(m_glesService2->getMemoryNode());

        auto noapiService1SignalFence = m_noapiService1->getSignalFence();
        auto glesService1SignalFence = m_glesService1->getSignalFence();
        auto glesService2SignalFence = m_glesService2->getSignalFence();

        m_noapiService1->addWaitFence(glesService2SignalFence);
        m_glesService1->addWaitFence(noapiService1SignalFence);
        m_glesService2->addWaitFence(glesService1SignalFence);
    }

    if (serviceCase == 1)
    {
        {
            m_noapiService1 = std::make_unique<JuneNoApiService1>(JuneServiceDescriptor{ .fps = 30,
                                                                                         .width = m_width,
                                                                                         .height = m_height,
                                                                                         .windowHandle = nullptr,
                                                                                         .appPath = m_appPath,
                                                                                         .appDir = m_appDir,
                                                                                         .appHandle = m_handle });
            m_noapiService1->start(JuneServiceStartDescriptor{
                .callback = [this]() {
                    m_noapiService1Ready = true;
                } });
        }

        {
            m_glesService1 = std::make_unique<JuneGLESService1>(JuneServiceDescriptor{ .fps = 120,
                                                                                       .width = m_width,
                                                                                       .height = m_height,
                                                                                       .windowHandle = nullptr,
                                                                                       .appPath = m_appPath,
                                                                                       .appDir = m_appDir,
                                                                                       .appHandle = m_handle });
            m_glesService1->start(JuneServiceStartDescriptor{
                .callback = [this]() {
                    m_glesService1Ready = true;
                } });
        }

        {
            m_vulkanService2 = std::make_unique<JuneVulkanService2>(JuneServiceDescriptor{ .fps = 30,
                                                                                           .width = m_width,
                                                                                           .height = m_height,
                                                                                           .windowHandle = getWindowHandle(),
                                                                                           .appPath = m_appPath,
                                                                                           .appDir = m_appDir,
                                                                                           .appHandle = m_handle });

            m_vulkanService2->start(JuneServiceStartDescriptor{
                .callback = [this]() {
                    m_vulkanService2Ready = true;
                } });
        }

        while (!m_noapiService1Ready ||
               !m_glesService1Ready ||
               !m_vulkanService2Ready)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
#if defined(__ANDROID__) || defined(ANDROID)
        for (auto aHardwareBuffer : m_aHardwareBuffers)
        {
            static_cast<JuneNoApiService1*>(m_noapiService1.get())->addAHardwareBuffer(aHardwareBuffer);
        }
#endif

        m_noapiService1->addSharedMemory(m_sharedMemories[0]);
        m_glesService1->addSharedMemory(m_sharedMemories[0]);
        m_vulkanService2->addSharedMemory(m_sharedMemories[0]);

        m_glesService1->connectMemoryNode(m_noapiService1->getMemoryNode());
        m_vulkanService2->connectMemoryNode(m_glesService1->getMemoryNode());
        m_noapiService1->connectMemoryNode(m_vulkanService2->getMemoryNode());

        auto noapiService1SignalFence = m_noapiService1->getSignalFence();
        auto glesService1SignalFence = m_glesService1->getSignalFence();
        auto vulkanService2SignalFence = m_vulkanService2->getSignalFence();

        m_noapiService1->addWaitFence(vulkanService2SignalFence);
        m_glesService1->addWaitFence(noapiService1SignalFence);
        m_vulkanService2->addWaitFence(glesService1SignalFence);
    }

    if (serviceCase == 2)
    {
        {
            m_noapiService1 = std::make_unique<JuneNoApiService1>(JuneServiceDescriptor{ .fps = 30,
                                                                                         .width = m_width,
                                                                                         .height = m_height,
                                                                                         .windowHandle = nullptr,
                                                                                         .appPath = m_appPath,
                                                                                         .appDir = m_appDir,
                                                                                         .appHandle = m_handle });
            m_noapiService1->start(JuneServiceStartDescriptor{
                .callback = [this]() {
                    m_noapiService1Ready = true;
                } });
        }

        {
            m_vulkanService1 = std::make_unique<JuneVulkanService1>(JuneServiceDescriptor{ .fps = 30,
                                                                                           .width = m_width,
                                                                                           .height = m_height,
                                                                                           .windowHandle = nullptr,
                                                                                           .appPath = m_appPath,
                                                                                           .appDir = m_appDir,
                                                                                           .appHandle = m_handle });

            m_vulkanService1->start(JuneServiceStartDescriptor{
                .callback = [this]() {
                    m_vulkanService1Ready = true;
                } });
        }

        {
            m_glesService2 = std::make_unique<JuneGLESService2>(JuneServiceDescriptor{ .fps = 120,
                                                                                       .width = m_width,
                                                                                       .height = m_height,
                                                                                       .windowHandle = getWindowHandle(),
                                                                                       .appPath = m_appPath,
                                                                                       .appDir = m_appDir,
                                                                                       .appHandle = m_handle });
            m_glesService2->start(JuneServiceStartDescriptor{
                .callback = [this]() {
                    m_glesService2Ready = true;
                } });
        }

        while (!m_noapiService1Ready ||
               !m_glesService2Ready ||
               !m_vulkanService1Ready)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
#if defined(__ANDROID__) || defined(ANDROID)
        for (auto aHardwareBuffer : m_aHardwareBuffers)
        {
            static_cast<JuneNoApiService1*>(m_noapiService1.get())->addAHardwareBuffer(aHardwareBuffer);
        }
#endif

        m_vulkanService1->addSharedMemory(m_sharedMemories[0]);
        m_glesService2->addSharedMemory(m_sharedMemories[0]);
        m_noapiService1->addSharedMemory(m_sharedMemories[0]);

        auto noapiService1SignalFence = m_noapiService1->getSignalFence();
        auto glesService2SignalFence = m_glesService2->getSignalFence();
        auto vulkanService1SignalFence = m_vulkanService1->getSignalFence();

        m_noapiService1->addWaitFence(glesService2SignalFence);
        m_vulkanService1->addWaitFence(noapiService1SignalFence);
        m_glesService2->addWaitFence(vulkanService1SignalFence);
    }
}

void JuneTriangleSample::createSharedMemories()
{

#if defined(__ANDROID__) || defined(ANDROID)
    {
        for (size_t i = 0; i < 3; ++i)
        {
            AHardwareBuffer* aHardwareBuffer = nullptr;

            AHardwareBuffer_Desc aHardwareBufferDesc = {
                .width = m_width,
                .height = m_height,
                .layers = 1,
                .format = AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM,
                .usage = AHARDWAREBUFFER_USAGE_GPU_SAMPLED_IMAGE | AHARDWAREBUFFER_USAGE_GPU_COLOR_OUTPUT | AHARDWAREBUFFER_USAGE_CPU_WRITE_RARELY
            };

            int result = AHardwareBuffer_allocate(&aHardwareBufferDesc, &aHardwareBuffer);
            if (result != 0)
            {
                spdlog::error("Failed to allocate AHardwareBuffer: {}", result);
                return;
            }

            m_aHardwareBuffers.push_back(aHardwareBuffer);

            JuneSharedMemoryAHardwareBufferImportDescriptor aHardwareBufferImportDescriptor{
                .chain = { .sType = JuneSType_SharedMemoryAHardwareBufferImportDescriptor },
                .aHardwareBuffer = aHardwareBuffer
            };

            JuneSharedMemoryImportDescriptor juneSharedMemoryImportDescriptor{
                .nextInChain = &aHardwareBufferImportDescriptor.chain,
                .label = { .data = "JuneTriangleSampleSharedMemoryAHardwareBuffer", .length = 32 }
            };

            m_sharedMemories.push_back(m_juneAPI.InstanceImportSharedMemory(m_juneInstance, &juneSharedMemoryImportDescriptor));
        }
    }
#endif
}

} // namespace jipu