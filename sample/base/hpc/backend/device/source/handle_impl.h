#pragma once

#include "device/handle.h"

namespace hpc
{
namespace device
{

class HandleImpl : public Handle
{
public:
    static std::unique_ptr<Handle> create(const char* path);

public:
    explicit HandleImpl(const int fd);
    ~HandleImpl() override;

private:
    const int m_fd = -1;
};

} // namespace device
} // namespace hpc