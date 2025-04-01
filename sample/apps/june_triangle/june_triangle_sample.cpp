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

    m_service1 = std::make_unique<JuneGLESService1>(JuneServiceDescriptor{ .fps = 30,
                                                                           .width = m_width,
                                                                           .height = m_height,
                                                                           .windowHandle = nullptr });
    m_service1->start(JuneServiceStartDescriptor{
        .callback = [this]() {
            m_service1Ready = true;
        } });

    m_service2 = std::make_unique<JuneGLESService2>(JuneServiceDescriptor{ .fps = 30,
                                                                           .width = m_width,
                                                                           .height = m_height,
                                                                           .windowHandle = getWindowHandle() });
    m_service2->start(JuneServiceStartDescriptor{
        .callback = [this]() {
            m_service2Ready = true;
        } });

    //    m_service3 = std::make_unique<JuneGLESService3>(JuneServiceDescriptor{ .fps = 30,
    //                                                                              .width = m_width,
    //                                                                              .height = m_height,
    //                                                                              .windowHandle = getWindowHandle() });
    //
    //    m_service3->start(JuneServiceStartDescriptor{
    //        .callback = [this]() {
    //            m_service3Ready = true;
    //        } });

    while (!m_service1Ready || !m_service2Ready)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    auto service1SharingObject = m_service1->getSharingObject();
    m_service2->setSharedObjects(service1SharingObject);
}

} // namespace jipu