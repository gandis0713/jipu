#pragma once

#include <memory>

namespace hpc
{
namespace backend
{

class Handle
{
public:
    static std::unique_ptr<Handle> create(const char* path);

public:
    explicit Handle(const int fd);
    virtual ~Handle();

    Handle(const Handle&) = delete;
    Handle& operator=(const Handle&) = delete;

public:
    int fd();

private:
    const int m_fd = -1;
};

} // namespace backend
} // namespace hpc