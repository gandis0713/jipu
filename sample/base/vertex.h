#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

namespace vkt
{

struct Vertex
{
    glm::vec3 pos;
    glm::vec2 texCoord;

    bool operator==(const Vertex& other) const;
};
} // namespace vkt

namespace std
{
template <>
struct hash<vkt::Vertex>
{
    size_t operator()(vkt::Vertex const& vertex) const
    {
        return (hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec2>()(vertex.texCoord) << 1)) >> 1;
    }
};
} // namespace std
