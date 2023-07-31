#pragma once

#include <glm/glm.hpp>

namespace vkt
{

struct Vertex
{
    glm::vec3 pos;
    glm::vec2 texCoord;

    bool operator==(const Vertex& other) const;
};
} // namespace vkt