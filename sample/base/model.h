#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <filesystem>
#include <vector>

namespace vkt
{

struct Vertex
{
    glm::vec3 pos;
    glm::vec2 texCoord;
};

struct Polygon
{
    std::vector<Vertex> vertices{};
    std::vector<uint16_t> indices{};
};

Polygon loadOBJ(const std::filesystem::path& path);

} // namespace vkt