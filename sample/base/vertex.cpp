#include "vertex.h"

namespace vkt
{

bool Vertex::operator==(const Vertex& other) const
{
    return pos == other.pos && texCoord == other.texCoord;
}

} // namespace vkt