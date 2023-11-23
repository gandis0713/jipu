#include "vertex.h"

namespace jipu
{

bool Vertex::operator==(const Vertex& other) const
{
    return pos == other.pos && normal == other.normal && tangent == other.tangent && texCoord == other.texCoord;
}

} // namespace jipu