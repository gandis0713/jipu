#include "june_triangle_sample.h"

#include "june_gles_service1.h"
#include "june_gles_service2.h"
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

    uint32_t serviceCase = 2;

    if (serviceCase == 0)
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

        while (!m_glesService1Ready ||
               !m_glesService2Ready)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        auto glesService1SharingObject = m_glesService1->getSharingObject();
        m_glesService2->setSharedObjects(glesService1SharingObject);
    }

    if (serviceCase == 1)
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
            !m_vulkanService1Ready ||
            !m_vulkanService2Ready)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        auto vulkanService1SharingObject = m_vulkanService1->getSharingObject();
        m_vulkanService2->setSharedObjects(vulkanService1SharingObject);
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
}

} // namespace jipu