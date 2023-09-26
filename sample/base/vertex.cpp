#include "vertex.h"

namespace vkt
{

bool Vertex::operator==(const Vertex& other) const
{
    return pos == other.pos && normal == other.normal && tangent == other.tangent && texCoord == other.texCoord;
}

} // namespace vkt