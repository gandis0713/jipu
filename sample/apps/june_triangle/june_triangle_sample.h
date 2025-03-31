#pragma once

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
};

} // namespace jipu