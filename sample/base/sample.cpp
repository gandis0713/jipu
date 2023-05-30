#include "sample.h"

Sample::Sample(int width, int height, const std::string& title, const char* path)
    : Window(width, height, title)
    , m_path(std::filesystem::path(path))
{
}