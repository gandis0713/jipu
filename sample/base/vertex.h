#pragma once

#include <glm/glm.hpp>

namespace jipu
{

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec4 tangent;
    glm::vec2 texCoord;

    bool operator==(const Vertex& other) const;
};
} // namespace jipu