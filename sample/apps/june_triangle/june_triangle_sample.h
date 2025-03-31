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
    JuneSharedMemory m_juneSharedMemory{ nullptr };
};

} // namespace jipu