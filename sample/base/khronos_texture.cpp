#include "khronos_texture.h"

#include <spdlog/spdlog.h>
#include <stdexcept>

namespace vkt
{

KTX::KTX(const std::filesystem::path& path)
{
    ktxResult ret = ktxTexture_CreateFromNamedFile(path.string().c_str(),
                                                   KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
                                                   &m_texture);
    if (ret != KTX_SUCCESS)
    {
        throw std::runtime_error("Failed to load KTX from file.");
    }
}

KTX::KTX(void* buf, uint64_t len)
{
    ktxResult ret = ktxTexture_CreateFromMemory(static_cast<const ktx_uint8_t*>(buf),
                                                len,
                                                KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
                                                &m_texture);
    if (ret != KTX_SUCCESS)
    {
        throw std::runtime_error("Failed to load KTX from memory.");
    }
}

KTX::~KTX()
{
    if (m_texture != nullptr)
        ktxTexture_Destroy(m_texture);
}

void* KTX::getPixels() const
{
    return ktxTexture_GetData(m_texture);
}

int KTX::getWidth() const
{
    return m_texture->baseWidth;
}

int KTX::getHeight() const
{
    return m_texture->baseHeight;
}

int KTX::getChannel() const
{
    return ktxTexture_GetElementSize(m_texture);
}

} // namespace vkt