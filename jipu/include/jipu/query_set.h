#pragma once

#include "export.h"

#include <cstdint>

namespace jipu
{

enum class QueryType
{
    kOcclusion,
    kTimestamp,
};

struct QuerySetDescriptor
{
    QueryType type;
    uint32_t count;
};

class JIPU_EXPORT QuerySet
{
public:
    virtual ~QuerySet() = default;

    QuerySet(const QuerySet&) = delete;
    QuerySet& operator=(const QuerySet&) = delete;

protected:
    QuerySet() = default;
};

} // namespace jipu