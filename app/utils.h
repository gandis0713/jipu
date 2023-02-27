#pragma once

#include <vector>
#include <filesystem>

namespace vkt::utils
{

std::vector<char> readFile(const std::filesystem::path& file_path);

} // namespace vkt::utils
