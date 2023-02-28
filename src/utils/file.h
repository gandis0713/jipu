#pragma once

#include <filesystem>
#include <vector>

namespace vkt::utils
{

std::vector<char> readFile(const std::filesystem::path& file_path);

} // namespace vkt::utils
