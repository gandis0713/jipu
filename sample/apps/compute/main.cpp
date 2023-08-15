
#include "sample.h"

#include "vkt/gpu/buffer.h"
#include "vkt/gpu/device.h"
#include "vkt/gpu/driver.h"
#include "vkt/gpu/physical_device.h"
#include "vkt/gpu/surface.h"
#include "vkt/gpu/swapchain.h"

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

class ComputeSample : public Sample
{
public:
    ComputeSample() = delete;
    ComputeSample(const SampleDescriptor& descriptor);
    ~ComputeSample() override;

public:
    void init() override;
    void draw() override;

private:
    void createDriver();
    void createPhysicalDevice();
    void createDevice();

private:
    std::unique_ptr<Buffer> m_vertexBuffer = nullptr;
    std::unique_ptr<Buffer> m_indexBUffer = nullptr;

    std::unique_ptr<Driver> m_driver = nullptr;
    std::unique_ptr<PhysicalDevice> m_physicalDevice = nullptr;
    std::unique_ptr<Device> m_device = nullptr;
    std::unique_ptr<Surface> m_surface = nullptr;
    std::unique_ptr<Swapchain> m_swapchain = nullptr;
};

ComputeSample::ComputeSample(const SampleDescriptor& descriptor)
    : Sample(descriptor)
{
}

ComputeSample::~ComputeSample()
{
}

void ComputeSample::init()
{
    createDriver();
    createPhysicalDevice();
    createDevice();
}

void ComputeSample::draw()
{
}

void ComputeSample::createDriver()
{
    DriverDescriptor descriptor{ .type = DriverType::VULKAN };
    m_driver = Driver::create(descriptor);
}

void ComputeSample::createPhysicalDevice()
{
    if (!m_driver)
        throw std::runtime_error("The driver instance is null pointer.");

    PhysicalDeviceDescriptor descriptor{ .index = 0 }; // TODO: use first physical device.
    m_physicalDevice = m_driver->createPhysicalDevice(descriptor);
}

void ComputeSample::createDevice()
{
    if (!m_physicalDevice)
        throw std::runtime_error("The physical device instance is null pointer.");

    DeviceDescriptor descriptor{};
    m_device = m_physicalDevice->createDevice(descriptor);
}

} // namespace vkt

#if defined(__ANDROID__) || defined(ANDROID)

void android_main(struct android_app* app)
{
    vkt::SampleDescriptor descriptor{
        { 1000, 2000, "Compute Shader Sample", app },
        ""
    };

    vkt::ComputeSample sample(descriptor);

    sample.exec();
}

#else

int main(int argc, char** argv)
{
    spdlog::set_level(spdlog::level::trace);

    vkt::SampleDescriptor descriptor{
        { 800, 600, "Compute Shader Sample", nullptr },
        argv[0]
    };

    vkt::ComputeSample sample(descriptor);

    return sample.exec();
}

#endif
