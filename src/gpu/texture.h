#pragma once

namespace vkt
{

class Device;

struct TextureCreateInfo
{
};

class Texture
{
public:
    Texture() = delete;
    Texture(Device* device, TextureCreateInfo info);
    virtual ~Texture() = default;

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

protected:
    Device* m_device{ nullptr };
};

} // namespace vkt