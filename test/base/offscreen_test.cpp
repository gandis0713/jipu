
#include "offscreen_test.h"

using namespace jipu;

void OffscreenTest::SetUp()
{
    m_width = 1280;
    m_height = 720;

    DriverDescriptor driverDescriptor;
    driverDescriptor.type = DriverType::VULKAN;
    m_driver = Driver::create(driverDescriptor);
    EXPECT_NE(nullptr, m_driver);

    m_physicalDevices = m_driver->getPhysicalDevices();
    EXPECT_NE(0, m_physicalDevices.size());

    PhysicalDevice* physicalDevice = m_physicalDevices[0].get();
    DeviceDescriptor deviceDescriptor{};
    m_device = physicalDevice->createDevice(deviceDescriptor);
    EXPECT_NE(nullptr, m_device);

#if defined(__ANDROID__) || defined(ANDROID)
    TextureFormat format = TextureFormat::kRGBA_8888_UInt_Norm_SRGB;
#else
    TextureFormat format = TextureFormat::kBGRA_8888_UInt_Norm_SRGB;
#endif

    TextureDescriptor renderViewDescriptor{};
    renderViewDescriptor.format = format;
    renderViewDescriptor.type = TextureType::k2D;
    renderViewDescriptor.usage = TextureUsageFlagBits::kColorAttachment;
    renderViewDescriptor.width = m_width;
    renderViewDescriptor.height = m_height;
    renderViewDescriptor.mipLevels = 1;
    renderViewDescriptor.sampleCount = 1;

    m_renderView = m_device->createTexture(renderViewDescriptor);
    EXPECT_NE(nullptr, m_renderView);
}

void OffscreenTest::TearDown()
{
    m_renderView.reset();
    m_device.reset();
    m_physicalDevices.clear();
    m_driver.reset();
}