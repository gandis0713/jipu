#pragma once

#include "vertex.h"

#include <filesystem>
#include <vector>

namespace vkt
{

struct Polygon
{
    std::vector<Vertex> vertices{};
    std::vector<uint16_t> indices{}; // TODO: The type of index must be uint16_t. please check it.
};

Polygon loadOBJ(const std::filesystem::path& path);
Polygon loadOBJ(void* buf, uint64_t len);
Polygon loadGLTF(const std::filesystem::path& path);
Polygon loadGLTF(void* buf, uint64_t len);

} // namespace vkt