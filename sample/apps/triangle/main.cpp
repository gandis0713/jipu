

#include "sample.h"

#include "vkt/gpu/device.h"
#include "vkt/gpu/driver.h"
#include "vkt/gpu/physical_device.h"
#include "vkt/gpu/surface.h"

#include <spdlog/spdlog.h>

#if defined(__ANDROID__) || defined(ANDROID)

    // GameActivity's C/C++ code
    #include <game-activity/GameActivity.cpp>
    #include <game-text-input/gametextinput.cpp>

// // Glue from GameActivity to android_main()
// // Passing GameActivity event from main thread to app native thread.
extern "C"
{
    #include <game-activity/native_app_glue/android_native_app_glue.c>
}

#endif

namespace vkt
{

class TriangleSample : public Sample
{
public:
    TriangleSample() = delete;
    TriangleSample(const SampleDescriptor& descriptor);
    ~TriangleSample() override;

    void init() override;
    void draw() override;

private:
    void createDevier();
    void createPhysicalDevice();
    void createSurface();
    void createDevice();

private:
    std::unique_ptr<Driver> m_driver = nullptr;
    std::unique_ptr<PhysicalDevice> m_physicalDevice = nullptr;
    std::unique_ptr<Surface> m_surface = nullptr;
    std::unique_ptr<Device> m_device = nullptr;
};

TriangleSample::TriangleSample(const SampleDescriptor& descriptor)
    : Sample(descriptor)
{
}

TriangleSample::~TriangleSample()
{
    m_device.reset();
    m_surface.reset();
    m_physicalDevice.reset();
    m_driver.reset();
}

void TriangleSample::init()
{
    createDevier();
    createPhysicalDevice();
    createSurface();
    createDevice();
}

void TriangleSample::draw()
{
}

void TriangleSample::createDevier()
{
    DriverDescriptor descriptor{};
    descriptor.type = DriverType::VULKAN;

    m_driver = Driver::create(descriptor);
}

void TriangleSample::createPhysicalDevice()
{
    PhysicalDeviceDescriptor descriptor{};
    descriptor.index = 0; // TODO: select device.

    m_physicalDevice = m_driver->createPhysicalDevice(descriptor);
}

void TriangleSample::createDevice()
{
    SurfaceDescriptor descriptor{};
    descriptor.windowHandle = getWindowHandle();

    m_surface = m_driver->createSurface(descriptor);
}

void TriangleSample::createSurface()
{
    DeviceDescriptor descriptor{};

    m_device = m_physicalDevice->createDevice(descriptor);
}

} // namespace vkt

#if defined(__ANDROID__) || defined(ANDROID)

void android_main(struct android_app* app)
{
    vkt::SampleDescriptor descriptor{
        { 1000, 2000, "Triangle", app },
        ""
    };

    vkt::TriangleSample sample(descriptor);

    sample.exec();
}

#else

int main(int argc, char** argv)
{
    spdlog::set_level(spdlog::level::trace);

    vkt::SampleDescriptor descriptor{
        { 800, 600, "Triangle", nullptr },
        argv[0]
    };

    vkt::TriangleSample sample(descriptor);

    return sample.exec();
}

#endif
