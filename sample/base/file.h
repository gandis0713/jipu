#pragma once

#include <filesystem>
#include <vector>

namespace vkt::utils
{

std::vector<char> readFile(void* platformContext, const std::filesystem::path& filePath);

} // namespace vkt::utils
