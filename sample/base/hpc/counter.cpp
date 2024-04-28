#include "counter.h"

namespace jipu
{
namespace hpc
{

void Counter::addKind(Kind kind)
{
    m_kinds.insert(kind);
}

} // namespace hpc
} // namespace jipu