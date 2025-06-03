#pragma once

#include "june/june.h"

#include <mutex>
#include <sigslot/signal.hpp>
#include <unordered_set>

namespace jipu
{

class JuneMemoryNode
{
public:
    JuneMemoryNode() = default;
    JuneMemoryNode(JuneSharedMemory sharedMemory);
    ~JuneMemoryNode() = default;

    JuneMemoryNode(const JuneMemoryNode&) = delete;
    JuneMemoryNode& operator=(const JuneMemoryNode&) = delete;

public:
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
    JuneSharedMemory m_sharedMemory;
    std::mutex m_accessMutex{};
    std::unordered_set<JuneMemoryNode*> m_inputs{};
    std::unordered_set<JuneMemoryNode*> m_outputs{};
    sigslot::signal<JuneMemoryNode*> m_signal;
};

} // namespace jipu