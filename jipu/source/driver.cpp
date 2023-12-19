#include "jipu/driver.h"

#include "source/vulkan/vulkan_driver.h"

#if defined(__ANDROID__) || defined(ANDROID)
    #include "spdlog/sinks/android_sink.h"
#else
    #include "spdlog/sinks/stdout_color_sinks.h"
#endif
#include <spdlog/spdlog.h>

namespace jipu
{

std::unique_ptr<Driver> Driver::create(const DriverDescriptor& descriptor)
{
    switch (descriptor.type)
    {
    case DriverType::VULKAN:
        return std::make_unique<VulkanDriver>(descriptor);
    default:
        spdlog::error("Unsupported driver type requested");
        return nullptr;
    }

    return nullptr;
}

Driver::Driver(const DriverDescriptor& descriptor)
{

#if defined(__ANDROID__) || defined(ANDROID)
    std::string tag = "spdlog-android";
    auto logger = spdlog::android_logger_mt("jipu");
#else
    auto logger = spdlog::stdout_color_mt("jipu");
#endif
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::trace);
}

Driver::~Driver()
{
    spdlog::set_default_logger(nullptr);
}

} // namespace jipu
