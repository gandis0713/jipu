#include "file.h"

#include <fmt/format.h>

#include <fstream>

namespace vkt::utils
{

std::vector<char> readFile(const std::filesystem::path& file_path)
{
    std::ifstream file(file_path, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error(fmt::format("Failed to open file: {}", std::string(file_path.c_str())));
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

} // namespace vkt::utils
