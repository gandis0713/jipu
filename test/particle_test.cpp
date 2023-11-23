#include "particle_test.h"

#include "jipu/gpu/device.h"
#include "jipu/gpu/driver.h"
#include "jipu/gpu/physical_device.h"
#include "jipu/gpu/surface.h"

using namespace jipu;

TEST_F(ParticleTest, test)
{
    DriverDescriptor driverDescriptor;
    driverDescriptor.type = DriverType::VULKAN;
    auto driver = Driver::create(driverDescriptor);
    EXPECT_NE(nullptr, driver);

    PhysicalDeviceDescriptor physicalDeviceDescriptor;
    physicalDeviceDescriptor.index = 0; // TODO:
    auto physicalDevice = driver->createPhysicalDevice(physicalDeviceDescriptor);
    EXPECT_NE(nullptr, physicalDevice);

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
