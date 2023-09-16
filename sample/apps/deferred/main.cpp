#include "sample.h"

#include "vkt/gpu/driver.h"
#include "vkt/gpu/physical_device.h"

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

class DeferredSample : public Sample
{
public:
    DeferredSample() = delete;
    DeferredSample(const SampleDescriptor& descriptor);
    ~DeferredSample() override;

public:
    void init() override;
    void draw() override;

private:
    void createDriver();
    void createPhysicalDevice();

private:
    std::unique_ptr<Driver> m_driver = nullptr;
    std::unique_ptr<PhysicalDevice> m_physicalDevice = nullptr;
};

DeferredSample::DeferredSample(const SampleDescriptor& descriptor)
    : Sample(descriptor)
{
    // Do not call init function.
}

DeferredSample::~DeferredSample()
{
}

void DeferredSample::init()
{
    createDriver();
    createPhysicalDevice();
}

void DeferredSample::draw()
{
}

void DeferredSample::createDriver()
{
    DriverDescriptor descriptor;
    descriptor.type = DriverType::VULKAN;
    m_driver = Driver::create(descriptor);
}

void DeferredSample::createPhysicalDevice()
{
    PhysicalDeviceDescriptor descriptor;
    descriptor.index = 0; // TODO: find index from driver.
    m_physicalDevice = m_driver->createPhysicalDevice(descriptor);
}

} // namespace vkt

#if defined(__ANDROID__) || defined(ANDROID)

void android_main(struct android_app* app)
{
    vkt::SampleDescriptor descriptor{
        { 1000, 2000, "Deferred Sample", app },
        ""
    };

    vkt::DeferredSample sample(descriptor);

    sample.exec();
}

#else

int main(int argc, char** argv)
{
    spdlog::set_level(spdlog::level::trace);

    vkt::SampleDescriptor descriptor{
        { 800, 600, "Deferred Sample", nullptr },
        argv[0]
    };

    vkt::DeferredSample sample(descriptor);

    return sample.exec();
}

#endif