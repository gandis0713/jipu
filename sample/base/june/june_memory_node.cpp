#include "june_memory_node.h"

#include <spdlog/spdlog.h>

namespace jipu
{

JuneMemoryNode::JuneMemoryNode(const JuneMemoryNodeDescriptor& descriptor)
    : m_descriptor(descriptor)
    , m_waitAccessCount(descriptor.waitAccessCount)
{
}

const std::string& JuneMemoryNode::getLabel() const
{
    return m_descriptor.label;
}

JuneSharedMemory JuneMemoryNode::getSharedMemory() const
{
    return m_descriptor.sharedMemory;
}

void JuneMemoryNode::beginAccess()
{
    m_accessMutex.lock();
}

void JuneMemoryNode::endAccess()
{
    signal();
}

void JuneMemoryNode::connect(JuneMemoryNode* inputNode)
{
    addInput(inputNode);
    inputNode->addOutput(this);
}

void JuneMemoryNode::disconnect(JuneMemoryNode* inputNode)
{
    removeInput(inputNode);
    inputNode->removeOutput(this);
}

void JuneMemoryNode::addInput(JuneMemoryNode* node)
{
    m_inputs.insert(node);
}

void JuneMemoryNode::removeInput(JuneMemoryNode* node)
{
    m_inputs.erase(node);
}

void JuneMemoryNode::removeInputAll()
{
    m_inputs.clear();
}

void JuneMemoryNode::addOutput(JuneMemoryNode* node)
{
    m_outputs.insert(node);
    m_signal.connect(&JuneMemoryNode::slot, node);
}

void JuneMemoryNode::removeOutput(JuneMemoryNode* node)
{
    m_outputs.erase(node);
    m_signal.disconnect(&JuneMemoryNode::slot, node);
}

void JuneMemoryNode::removeOutputAll()
{
    for (auto& output : m_outputs)
    {
        m_signal.disconnect(&JuneMemoryNode::slot, output);
    }
    m_outputs.clear();
}

void JuneMemoryNode::signal()
{
    m_signal(this);
}

void JuneMemoryNode::slot(JuneMemoryNode* memory)
{
    std::lock_guard<std::mutex> lock(m_countMutex);
    if (m_waitAccessCount > 0)
    {
        m_waitAccessCount--;
    }

    spdlog::trace("input: {}, {} Memory node access count: {}", memory->getLabel(), m_descriptor.label, m_waitAccessCount);

    if (m_waitAccessCount > 0)
    {
        return;
    }

    m_waitAccessCount = m_descriptor.waitAccessCount;
    m_accessMutex.unlock();
}

} // namespace jipu