#pragma once

#include "export.h"

namespace jipu
{

struct QuerySetDescriptor
{
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