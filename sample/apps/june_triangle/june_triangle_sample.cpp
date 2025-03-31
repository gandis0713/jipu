#include "june_triangle_sample.h"

#include "june_gles_service1.h"
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
    m_services.push_back(std::make_unique<JuneGLESService1>(JuneServiceDescriptor{ .fps = 30,
                                                                                   .width = m_width,
                                                                                   .height = m_height,
                                                                                   .windowHandle = getWindowHandle() }));

    for (auto& service : m_services)
    {
        service->start();
    }
}

} // namespace jipu