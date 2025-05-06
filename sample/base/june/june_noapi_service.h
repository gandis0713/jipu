#pragma once

#include "jipu/common/dylib.h"
#include "june/june.h"
#include "june_api.h"
#include "june_service.h"

#if defined(__ANDROID__) || defined(ANDROID)
#include <android/hardware_buffer.h>
#include <android/native_window.h>
#include <android/sync.h>
#endif

namespace jipu
{

class JuneNoApiService : public JuneService
{

public:
    JuneNoApiService(const JuneServiceDescriptor& descriptor);
    ~JuneNoApiService();

protected:
    void begin() override;
    void end() override;

    void createApiContext(const std::string& label) override;
};

} // namespace jipu