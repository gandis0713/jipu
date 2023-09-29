#include "model.h"

#include <fmt/format.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>
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

vkt::Polygon generatePolygonFromOBJ(const tinyobj::attrib_t& attrib, const std::vector<tinyobj::shape_t>& shapes)
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

vkt::Polygon generatePolygonFromGLTF(const tinygltf::Model& model, const tinygltf::Mesh& mesh)
{
    vkt::Polygon polygon{};

    // Iterate through all primitives of this node's mesh
    for (size_t i = 0; i < mesh.primitives.size(); i++)
    {
        const tinygltf::Primitive& glTFPrimitive = mesh.primitives[i];
        uint32_t firstIndex = static_cast<uint32_t>(polygon.indices.size());
        uint32_t vertexStart = static_cast<uint32_t>(polygon.vertices.size());
        // Vertices
        {
            const float* positionBuffer = nullptr;
            const float* normalsBuffer = nullptr;
            const float* tangentBuffer = nullptr;
            const float* texCoordsBuffer = nullptr;
            size_t vertexCount = 0;

            // Get buffer data for vertex positions
            if (glTFPrimitive.attributes.find("POSITION") != glTFPrimitive.attributes.end())
            {
                const tinygltf::Accessor& accessor = model.accessors[glTFPrimitive.attributes.find("POSITION")->second];
                const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];
                positionBuffer = reinterpret_cast<const float*>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                vertexCount = accessor.count;
            }
            // Get buffer data for vertex normals
            if (glTFPrimitive.attributes.find("NORMAL") != glTFPrimitive.attributes.end())
            {
                const tinygltf::Accessor& accessor = model.accessors[glTFPrimitive.attributes.find("NORMAL")->second];
                const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];
                normalsBuffer = reinterpret_cast<const float*>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
            }

            // Get buffer data for vertex tangent
            if (glTFPrimitive.attributes.find("TANGENT") != glTFPrimitive.attributes.end())
            {
                const tinygltf::Accessor& accessor = model.accessors[glTFPrimitive.attributes.find("TANGENT")->second];
                const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];
                tangentBuffer = reinterpret_cast<const float*>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
            }

            // Get buffer data for vertex texture coordinates
            // glTF supports multiple sets, we only load the first one
            if (glTFPrimitive.attributes.find("TEXCOORD_0") != glTFPrimitive.attributes.end())
            {
                const tinygltf::Accessor& accessor = model.accessors[glTFPrimitive.attributes.find("TEXCOORD_0")->second];
                const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];
                texCoordsBuffer = reinterpret_cast<const float*>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
            }

            // Append data to model's vertex buffer
            for (size_t v = 0; v < vertexCount; v++)
            {
                vkt::Vertex vert{};
                vert.pos = glm::vec4(glm::make_vec3(&positionBuffer[v * 3]), 1.0f);
                vert.normal = glm::normalize(glm::vec3(normalsBuffer ? glm::make_vec3(&normalsBuffer[v * 3]) : glm::vec3(0.0f)));
                vert.tangent = glm::normalize(glm::vec4(tangentBuffer ? glm::make_vec4(&tangentBuffer[v * 4]) : glm::vec4(0.0f)));
                vert.texCoord = texCoordsBuffer ? glm::make_vec2(&texCoordsBuffer[v * 2]) : glm::vec3(0.0f);
                polygon.vertices.push_back(vert);
            }
        }
        // Indices
        {
            const tinygltf::Accessor& accessor = model.accessors[glTFPrimitive.indices];
            const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
            const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

            uint32_t indexCount = static_cast<uint32_t>(accessor.count);

            // glTF supports different component types of indices
            switch (accessor.componentType)
            {
            case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
            {
                const uint32_t* buf = reinterpret_cast<const uint32_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
                for (size_t index = 0; index < accessor.count; index++)
                {
                    polygon.indices.push_back(buf[index] + vertexStart);
                }
                break;
            }
            case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
            {
                const uint16_t* buf = reinterpret_cast<const uint16_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
                for (size_t index = 0; index < accessor.count; index++)
                {
                    polygon.indices.push_back(buf[index] + vertexStart);
                }
                break;
            }
            case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
            {
                const uint8_t* buf = reinterpret_cast<const uint8_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
                for (size_t index = 0; index < accessor.count; index++)
                {
                    polygon.indices.push_back(buf[index] + vertexStart);
                }
                break;
            }
            default:
                throw std::runtime_error(fmt::format("Index componenet type {} not supported.", accessor.componentType));
            }
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

    return generatePolygonFromOBJ(attrib, shapes);
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

    return generatePolygonFromOBJ(attrib, shapes);
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

    // for (const auto& mesh : model.meshes)
    const auto& mesh = model.meshes[1];

    return generatePolygonFromGLTF(model, mesh);
}

Polygon loadGLTF(void* buf, uint64_t len, const std::filesystem::path& baseDir)
{
    using namespace tinygltf;

    Model model;
    TinyGLTF gltf;
    std::string err;
    std::string warn;

    // TODO
    if (!gltf.LoadASCIIFromString(&model, &err, &warn, static_cast<const char*>(buf), len, baseDir.string().c_str()))
    {
        throw std::runtime_error(warn + err);
    }

    // for (const auto& mesh : model.meshes)
    const auto& mesh = model.meshes[1];

    return generatePolygonFromGLTF(model, mesh);
}

} // namespace vkt