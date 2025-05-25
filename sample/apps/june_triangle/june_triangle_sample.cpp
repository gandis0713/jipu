#include "june_triangle_sample.h"

#include "june_gles_service1.h"
#include "june_gles_service2.h"
#include "june_noapi_service1.h"
#include "june_vulkan_service1.h"
#include "june_vulkan_service2.h"
#include "june_vulkan_service3.h"

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

    uint32_t serviceCase = 0;

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

        auto sharingMemories = m_noapiService1->getSharingMemories();
        for (auto sharingMemory : sharingMemories)
        {
            m_glesService1->addSharedMemory(sharingMemory);
            m_glesService2->addSharedMemory(sharingMemory);
        }

        auto noapiService1SignalFence = m_noapiService1->getSignalFence();
        auto glesService1SignalFence = m_glesService1->getSignalFence();
        auto glesService2SignalFence = m_glesService2->getSignalFence();

        m_noapiService1->addWaitFence(glesService1SignalFence);
        m_noapiService1->addWaitFence(glesService2SignalFence);

        m_glesService1->addWaitFence(noapiService1SignalFence);
        m_glesService1->addWaitFence(glesService2SignalFence);

        m_glesService2->addWaitFence(noapiService1SignalFence);
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
            m_vulkanService1 = std::make_unique<JuneVulkanService1>(JuneServiceDescriptor{ .fps = 30,
                                                                                           .width = m_width,
                                                                                           .height = m_height,
                                                                                           .windowHandle = getWindowHandle(),
                                                                                           .appPath = m_appPath,
                                                                                           .appDir = m_appDir,
                                                                                           .appHandle = m_handle });

            m_vulkanService1->start(JuneServiceStartDescriptor{
                .callback = [this]() {
                    m_vulkanService1Ready = true;
                } });
        }

        while (!m_noapiService1Ready ||
               !m_glesService1Ready ||
               !m_vulkanService1Ready)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        auto sharingMemories = m_noapiService1->getSharingMemories();
        m_glesService1->addSharedMemory(sharingMemories[0]);
        // m_glesService1->addSharedMemory(sharingMemories[1]);
        // m_vulkanService1->addSharedMemory(sharingMemories[1]);
        m_vulkanService1->addSharedMemory(sharingMemories[0]);

        auto noapiService1SignalFence = m_noapiService1->getSignalFence();
        auto glesService1SignalFence = m_glesService1->getSignalFence();
        auto vulkanService1SignalFence = m_vulkanService1->getSignalFence();

        m_noapiService1->addWaitFence(glesService1SignalFence);
        m_noapiService1->addWaitFence(vulkanService1SignalFence);

        m_glesService1->addWaitFence(noapiService1SignalFence);
        m_glesService1->addWaitFence(vulkanService1SignalFence);

        m_vulkanService1->addWaitFence(noapiService1SignalFence);
        m_vulkanService1->addWaitFence(glesService1SignalFence);
    }

    if (serviceCase == 2)
    {
        {
            m_vulkanService3 = std::make_unique<JuneVulkanService3>(JuneServiceDescriptor{ .fps = 30,
                                                                                           .width = m_width,
                                                                                           .height = m_height,
                                                                                           .windowHandle = getWindowHandle(),
                                                                                           .appPath = m_appPath,
                                                                                           .appDir = m_appDir,
                                                                                           .appHandle = m_handle });

            m_vulkanService3->start(JuneServiceStartDescriptor{
                .callback = [this]() {
                    m_vulkanService3Ready = true;
                } });
        }

        while (!m_vulkanService3Ready)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    if (serviceCase == 3)
    {
        {
            m_glesService1 = std::make_unique<JuneGLESService1>(JuneServiceDescriptor{ .fps = 30,
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

        while (
            !m_glesService1Ready ||
            !m_vulkanService2Ready)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        auto sharingMemories = m_glesService1->getSharingMemories();
        m_vulkanService2->addSharedMemory(sharingMemories[0]);

        auto glesService1SignalFence = m_glesService1->getSignalFence();
        auto vulkanService2SignalFence = m_vulkanService2->getSignalFence();

        m_glesService1->addWaitFence(vulkanService2SignalFence);
        m_vulkanService2->addWaitFence(glesService1SignalFence);
    }

    if (serviceCase == 4)
    {
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

        while (
            !m_vulkanService1Ready ||
            !m_glesService2Ready)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        auto sharingMemories = m_vulkanService1->getSharingMemories();
        m_glesService2->addSharedMemory(sharingMemories[0]);

        auto vulkanService1SignalFence = m_vulkanService1->getSignalFence();
        auto glesService2SignalFence = m_glesService2->getSignalFence();

        m_vulkanService1->addWaitFence(glesService2SignalFence);
        m_glesService2->addWaitFence(vulkanService1SignalFence);
    }
}

} // namespace jipu