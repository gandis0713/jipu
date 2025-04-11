#pragma once

#include "june/june.h"
#include "june_sample.h"

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
    std::unique_ptr<JuneService> m_glesService1{ nullptr };
    std::unique_ptr<JuneService> m_glesService2{ nullptr };
    std::unique_ptr<JuneService> m_vulkanService1{ nullptr };
    std::unique_ptr<JuneService> m_vulkanService2{ nullptr };
    std::unique_ptr<JuneService> m_vulkanService3{ nullptr };

    [[maybe_unused]] bool m_glesService1Ready{ false };
    [[maybe_unused]] bool m_glesService2Ready{ false };
    [[maybe_unused]] bool m_vulkanService1Ready{ false };
    [[maybe_unused]] bool m_vulkanService2Ready{ false };
    [[maybe_unused]] bool m_vulkanService3Ready{ false };
};

} // namespace jipu