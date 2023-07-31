#include "image.h"

namespace vkt
{

void* Image::getPixels() const
{
    return m_pixels;
}

int Image::getWidth() const
{
    return m_width;
}

int Image::getHeight() const
{
    return m_height;
}

int Image::getChannel() const
{
    return m_channel;
}

} // namespace vkt