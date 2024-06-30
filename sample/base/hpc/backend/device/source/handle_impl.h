#pragma once

#include "common/cast.h"
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

public:
    int fd();

private:
    const int m_fd = -1;
};
DOWN_CAST(HandleImpl, Handle)

} // namespace device
} // namespace hpc