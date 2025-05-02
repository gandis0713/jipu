#include "june_service1.h"

#include <random>
#include <spdlog/spdlog.h>
#include <thread>

namespace jipu
{

namespace
{

} // namespace

JuneNoApiService1::JuneNoApiService1(const JuneServiceDescriptor& descriptor)
    : JuneService(descriptor)
{
}

JuneNoApiService1::~JuneNoApiService1()
{
}

void JuneNoApiService1::begin()
{
    JuneService::begin();

    // Create June Instance
    {
        JuneInstanceDescriptor juneInstanceDescriptor{};
        m_juneInstance = m_juneAPI.CreateInstance(&juneInstanceDescriptor);
    }

    // Create Api Context
    {
        JuneNoApiContextDescriptor juneNoApiContextDescriptor{};
        juneNoApiContextDescriptor.chain.sType = JuneSType_NoApiContext;

        std::string label = "[NoApiService1]";
        JuneApiContextDescriptor juneApiContextDescriptor;
        juneApiContextDescriptor.nextInChain = &juneNoApiContextDescriptor.chain;
        juneApiContextDescriptor.label.data = label.data();
        juneApiContextDescriptor.label.length = label.length();
        m_juneApiContext = m_juneAPI.InstanceCreateApiContext(m_juneInstance, &juneApiContextDescriptor);
    }

    // Create Fence
    {
        JuneFenceDescriptor fenceDescriptor;
        m_fence = m_juneAPI.ApiContextCreateFence(m_juneApiContext, &fenceDescriptor);

        m_sharingObjects.fences.push_back(m_fence);
    }

    // Export AhardwareBuffer from Shared Memory
    {
    }
}

void JuneNoApiService1::work()
{
    {
        std::lock_guard<std::mutex> lock(m_sharedMutex);
        if (m_shared)
        {
            JuneSharedMemoryExportedEGLSyncKHRSyncObject waitExportedEGLSyncKHRSyncObject{};
            {
                JuneSharedMemorySyncInfo waitSyncInfo{};
                waitSyncInfo.fences = m_sharedObjects.fences.data();
                waitSyncInfo.fenceCount = m_sharedObjects.fences.size();

                waitExportedEGLSyncKHRSyncObject.chain.sType = JuneSType_SharedMemoryExportedEGLSyncKHRSyncObject;

                JuneSharedMemoryExportedSyncObject exportedSyncObject{};
                exportedSyncObject.nextInChain = &waitExportedEGLSyncKHRSyncObject.chain;

                JuneApiContextBeginMemoryAccessDescriptor descriptor{};
                descriptor.sharedMemory = m_sharedObjects.sharedMemory;
                descriptor.waitSyncInfo = &waitSyncInfo;
                descriptor.exportedSyncObject = &exportedSyncObject;

                m_juneAPI.ApiContextBeginMemoryAccess(m_juneApiContext, &descriptor);
            }
        }
    }

    spdlog::debug("no api service1 begin access");

    {
    }

    spdlog::debug("no api service1 end access");

    {
        std::lock_guard<std::mutex> lock(m_sharedMutex);
        if (m_shared)
        {
            JuneSharedMemoryExportedEGLSyncKHRSyncObject signalExportedEGLSyncKHRSyncObject{};
            {
                JuneSharedMemorySyncInfo signalSyncInfo{};
                signalSyncInfo.fences = m_sharingObjects.fences.data();
                signalSyncInfo.fenceCount = m_sharingObjects.fences.size();

                signalExportedEGLSyncKHRSyncObject.chain.sType = JuneSType_SharedMemoryExportedEGLSyncKHRSyncObject;

                JuneSharedMemoryExportedSyncObject exportedSyncObject{};
                exportedSyncObject.nextInChain = &signalExportedEGLSyncKHRSyncObject.chain;

                JuneApiContextEndMemoryAccessDescriptor descriptor{};
                descriptor.sharedMemory = m_sharedObjects.sharedMemory;
                descriptor.signalSyncInfo = &signalSyncInfo;
                descriptor.exportedSyncObject = &exportedSyncObject;

                m_juneAPI.ApiContextEndMemoryAccess(m_juneApiContext, &descriptor);
            }
        }
    }
}

JuneServiceShareObjects JuneNoApiService1::getSharingObject() const
{
    return m_sharingObjects;
}

void JuneNoApiService1::setSharedObjects(const JuneServiceShareObjects& sharedObjects)
{
    m_sharedObjects = sharedObjects;

    std::lock_guard<std::mutex> lock(m_sharedMutex);
    m_shared = true;
}

} // namespace jipu
