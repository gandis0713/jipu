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
    [[maybe_unused]] bool m_service1Ready{ false };
    [[maybe_unused]] bool m_service2Ready{ false };
    [[maybe_unused]] bool m_service3Ready{ false };
};

} // namespace jipu