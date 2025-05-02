#pragma once

#include "june/june.h"
#include "june/june_noapi_service.h"

namespace jipu
{

class JuneNoApiService1 : public JuneNoApiService
{

public:
    JuneNoApiService1(const JuneServiceDescriptor& descriptor);
    ~JuneNoApiService1();

    void begin() override;
    void work() override;

    JuneServiceShareObjects getSharingObject() const override;
    void setSharedObjects(const JuneServiceShareObjects& sharedObjects) override;

private:
    JuneApiContext m_juneApiContext{ nullptr };
    JuneFence m_fence{ nullptr };

#if defined(__ANDROID__) || defined(ANDROID)
private:
    AHardwareBuffer* m_aHardwareBuffer{ nullptr };
#endif
};

} // namespace jipu