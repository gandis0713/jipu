#include "june_memory_node.h"

#include <spdlog/spdlog.h>

namespace jipu
{

JuneMemoryNode::JuneMemoryNode(JuneSharedMemory sharedMemory)
    : m_sharedMemory(sharedMemory)
{
    if (!m_sharedMemory)
    {
        spdlog::error("Invalid shared memory provided to JuneMemoryNode.");
        return;
    }
}

JuneSharedMemory JuneMemoryNode::getSharedMemory() const
{
    return m_sharedMemory;
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
    m_accessMutex.unlock();
}

} // namespace jipu