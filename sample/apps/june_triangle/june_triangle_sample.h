#pragma once

#include "june/june.h"
#include "june/june_sample.h"

#if defined(__ANDROID__) || defined(ANDROID)
#include <android/hardware_buffer.h>
#include <android/native_window.h>
#endif

namespace jipu
{

class JuneTriangleSample : public JuneSample
{
public:
    JuneTriangleSample() = delete;
    JuneTriangleSample(const JuneSampleDescriptor& descriptor);
    ~JuneTriangleSample() override;

    void init() override;

private:
    void createSharedMemories();

private:
    std::unique_ptr<JuneService> m_glesService1{ nullptr };
    std::unique_ptr<JuneService> m_glesService2{ nullptr };
    std::unique_ptr<JuneService> m_noapiService1{ nullptr };
    std::unique_ptr<JuneService> m_vulkanService1{ nullptr };
    std::unique_ptr<JuneService> m_vulkanService2{ nullptr };

    [[maybe_unused]] bool m_glesService1Ready{ false };
    [[maybe_unused]] bool m_glesService2Ready{ false };
    [[maybe_unused]] bool m_noapiService1Ready{ false };
    [[maybe_unused]] bool m_vulkanService1Ready{ false };
    [[maybe_unused]] bool m_vulkanService2Ready{ false };

#if defined(__ANDROID__) || defined(ANDROID)
    std::vector<AHardwareBuffer*> m_aHardwareBuffers{};
#endif
    std::vector<JuneSharedMemory> m_sharedMemories{};
};

} // namespace jipu