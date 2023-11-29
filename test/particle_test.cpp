#include "particle_test.h"

#include "jipu/device.h"
#include "jipu/driver.h"
#include "jipu/physical_device.h"
#include "jipu/surface.h"

using namespace jipu;

TEST_F(ParticleTest, test)
{
    DriverDescriptor driverDescriptor;
    driverDescriptor.type = DriverType::VULKAN;
    auto driver = Driver::create(driverDescriptor);
    EXPECT_NE(nullptr, driver);

    auto physicalDevices = driver->getPhysicalDevices();
    EXPECT_NE(0, physicalDevices.size());

    PhysicalDevice* physicalDevice = physicalDevices[0].get();
    DeviceDescriptor deviceDescriptor{};
    auto device = physicalDevice->createDevice(deviceDescriptor);
    EXPECT_NE(nullptr, device);

    SurfaceDescriptor surfaceDescriptor{ .windowHandle = handle() };
    auto surface = driver->createSurface(surfaceDescriptor);
    EXPECT_NE(nullptr, surface);

    const uint64_t uniformBufferSize = sizeof(float);

    BufferDescriptor uniformBufferDescriptor{};
    uniformBufferDescriptor.size = uniformBufferSize;
    uniformBufferDescriptor.usage = BufferUsageFlagBits::kUniform;
    auto uniformBuffer = device->createBuffer(uniformBufferDescriptor);
    EXPECT_NE(nullptr, uniformBuffer);
}
