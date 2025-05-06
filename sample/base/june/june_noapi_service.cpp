#include "june_noapi_service.h"

namespace jipu
{

JuneNoApiService::JuneNoApiService(const JuneServiceDescriptor& descriptor)
    : JuneService(descriptor)
{
}

JuneNoApiService::~JuneNoApiService()
{
}

void JuneNoApiService::begin()
{
}

void JuneNoApiService::end()
{
}

void JuneNoApiService::createApiContext(const std::string& label)
{
    JuneNoApiContextDescriptor juneNoApiContextDescriptor{};
    juneNoApiContextDescriptor.chain.sType = JuneSType_NoApiContext;

    JuneApiContextDescriptor juneApiContextDescriptor;
    juneApiContextDescriptor.nextInChain = &juneNoApiContextDescriptor.chain;
    juneApiContextDescriptor.label.data = label.data();
    juneApiContextDescriptor.label.length = label.length();
    m_juneApiContext = m_juneAPI.InstanceCreateApiContext(m_juneInstance, &juneApiContextDescriptor);
}

} // namespace jipu
