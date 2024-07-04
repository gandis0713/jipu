#pragma once

#include "common/cast.h"
#include "hpc/backend/handle.h"

namespace hpc
{
namespace backend
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

} // namespace backend
} // namespace hpc