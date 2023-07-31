#pragma once

#include "image.h"

namespace vkt
{

class JPEGImage : public Image
{
public:
    JPEGImage(const std::filesystem::path& path);
    ~JPEGImage() override;
};

} // namespace vkt