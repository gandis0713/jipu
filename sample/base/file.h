#pragma once

#include <filesystem>
#include <vector>

namespace jipu::utils
{

std::vector<char> readFile(const std::filesystem::path& filePath, void* platformContext = nullptr);

} // namespace jipu::utils
