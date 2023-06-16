#pragma once

#include <filesystem>
#include <vector>

namespace vkt::utils
{

std::vector<char> readFile(const std::filesystem::path& filePath, void* platformContext = nullptr);

} // namespace vkt::utils
