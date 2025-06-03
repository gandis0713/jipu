#pragma once

#include "june/june.h"

#include <mutex>
#include <sigslot/signal.hpp>
#include <unordered_set>

namespace jipu
{

struct JuneMemoryNodeDescriptor
{
    std::string label;
    JuneSharedMemory sharedMemory = nullptr;
    uint16_t waitAccessCount = 0;
};

class JuneMemoryNode
{
public:
    JuneMemoryNode(const JuneMemoryNodeDescriptor& descriptor);
    ~JuneMemoryNode() = default;

    JuneMemoryNode(const JuneMemoryNode&) = delete;
    JuneMemoryNode& operator=(const JuneMemoryNode&) = delete;

public:
    const std::string& getLabel() const;
    JuneSharedMemory getSharedMemory() const;

public:
    void beginAccess();
    void endAccess();
    void connect(JuneMemoryNode* inputNode);
    void disconnect(JuneMemoryNode* inputNode);

private:
    void addInput(JuneMemoryNode* node);
    void removeInput(JuneMemoryNode* node);
    void removeInputAll();
    void addOutput(JuneMemoryNode* node);
    void removeOutput(JuneMemoryNode* node);
    void removeOutputAll();

private:
    void signal();
    void slot(JuneMemoryNode* memory);

private:
    const JuneMemoryNodeDescriptor m_descriptor{};
    std::mutex m_accessMutex{};
    std::mutex m_countMutex{};
    std::unordered_set<JuneMemoryNode*> m_inputs{};
    std::unordered_set<JuneMemoryNode*> m_outputs{};
    sigslot::signal<JuneMemoryNode*> m_signal;
    uint16_t m_waitAccessCount{ 0 };
};

} // namespace jipu