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

    // m_services.push_back(std::make_unique<JuneVulkanService1>(JuneServiceDescriptor{ .fps = 30,
    //                                                                                  .width = m_width,
    //                                                                                  .height = m_height,
    //                                                                                  .windowHandle = getWindowHandle() }));

    auto service1 = std::make_unique<JuneGLESService1>(JuneServiceDescriptor{ .fps = 30,
                                                                              .width = m_width,
                                                                              .height = m_height,
                                                                              .windowHandle = nullptr });
    service1->start(JuneServiceStartDescriptor{
        .callback = [this]() {
            m_service1Ready = true;
        } });
    m_services.push_back(std::move(service1));

    auto service2 = std::make_unique<JuneGLESService2>(JuneServiceDescriptor{ .fps = 30,
                                                                              .width = m_width,
                                                                              .height = m_height,
                                                                              .windowHandle = getWindowHandle() });
    service2->start(JuneServiceStartDescriptor{
        .callback = [this]() {
            m_service2Ready = true;
        } });
    m_services.push_back(std::move(service2));

    //    auto service3 = std::make_unique<JuneGLESService3>(JuneServiceDescriptor{ .fps = 30,
    //                                                                              .width = m_width,
    //                                                                              .height = m_height,
    //                                                                              .windowHandle = getWindowHandle() });
    //
    //    service3->start(JuneServiceStartDescriptor{
    //        .callback = [this]() {
    //            m_service3Ready = true;
    //        } });

    //    m_services.push_back(std::move(service3));

    while (!m_service1Ready || !m_service2Ready)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    m_services[1]->shareMemory(m_services[0]->getJuneSharedMemory("memory1"));
}

} // namespace jipu