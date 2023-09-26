#include "model.h"

#include <sstream>
#include <tiny_obj_loader.h>
#include <unordered_map>

#define TINYGLTF_IMPLEMENTATION
// #define STB_IMAGE_IMPLEMENTATION <- is in image.cpp
#define STB_IMAGE_WRITE_IMPLEMENTATION
// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.
#include "tiny_gltf.h"

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

Polygon loadGLTF(const std::filesystem::path& path)
{
    using namespace tinygltf;

    Model model;
    TinyGLTF gltf;
    std::string err;
    std::string warn;

    if (!gltf.LoadASCIIFromFile(&model, &err, &warn, path.string().c_str()))
    {
        throw std::runtime_error(warn + err);
    }

    // TODO: convert it to polygon.
    return {};
}

Polygon loadGLTF(void* buf, uint64_t len)
{
    using namespace tinygltf;

    Model model;
    TinyGLTF gltf;
    std::string err;
    std::string warn;

    // TODO
    // if (!gltf.LoadBinaryFromMemory(&model, &err, &warn, static_cast<const unsigned char*>(buf), len))
    // {
    //     throw std::runtime_error(warn + err);
    // }

    return {};
}

} // namespace vkt