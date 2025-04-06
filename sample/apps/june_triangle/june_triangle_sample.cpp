#include "june_triangle_sample.h"

#include "june_gles_service1.h"
#include "june_gles_service2.h"
#include "june_gles_service3.h"
#include "june_vulkan_service1.h"

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

    m_glesService1 = std::make_unique<JuneGLESService1>(JuneServiceDescriptor{ .fps = 30,
                                                                               .width = m_width,
                                                                               .height = m_height,
                                                                               .windowHandle = nullptr });
    m_glesService1->start(JuneServiceStartDescriptor{
        .callback = [this]() {
            m_glesService1Ready = true;
        } });

    m_glesService2 = std::make_unique<JuneGLESService2>(JuneServiceDescriptor{ .fps = 30,
                                                                               .width = m_width,
                                                                               .height = m_height,
                                                                               .windowHandle = getWindowHandle() });
    m_glesService2->start(JuneServiceStartDescriptor{
        .callback = [this]() {
            m_glesService2Ready = true;
        } });

    // m_glesService3 = std::make_unique<JuneGLESService3>(JuneServiceDescriptor{ .fps = 30,
    //                                                                        .width = m_width,
    //                                                                        .height = m_height,
    //                                                                        .windowHandle = nullptr });

    // m_glesService3->start(JuneServiceStartDescriptor{
    //     .callback = [this]() {
    //         m_glesService3Ready = true;
    //     } });

    m_vulkanService1 = std::make_unique<JuneVulkanService1>(JuneServiceDescriptor{ .fps = 30,
                                                                                   .width = m_width,
                                                                                   .height = m_height,
                                                                                   .windowHandle = nullptr });

    m_vulkanService1->start(JuneServiceStartDescriptor{
        .callback = [this]() {
            m_vulkanService1Ready = true;
        } });

    while (!m_glesService1Ready || !m_glesService2Ready || !m_vulkanService1Ready)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    auto service1SharingObject = m_glesService1->getSharingObject();
    m_glesService2->setSharedObjects(service1SharingObject);
}

} // namespace jipu