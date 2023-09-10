#include "model.h"

#include <sstream>
#include <tiny_obj_loader.h>
#include <unordered_map>

namespace
{

vkt::Polygon generatePolygon(const tinyobj::attrib_t& attrib, const std::vector<tinyobj::shape_t>& shapes)
{
    vkt::Polygon polygon{};

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            vkt::Vertex vertex{};

            vertex.pos = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.texCoord = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
            };

            polygon.vertices.push_back(vertex);
            polygon.indices.push_back(polygon.indices.size());
        }
    }

    return polygon;
}

} // namespace

namespace vkt
{

Polygon loadOBJ(const std::filesystem::path& path)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.string().c_str()))
    {
        throw std::runtime_error(warn + err);
    }

    return generatePolygon(attrib, shapes);
}

Polygon loadOBJ(void* buf, uint64_t len)
{
    std::stringstream ss;
    ss.write(static_cast<const char*>(buf), len);

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, &ss))
    {
        throw std::runtime_error(warn + err);
    }

    return generatePolygon(attrib, shapes);
}

} // namespace vkt