#pragma once

namespace vkt
{

enum class TextureFormat
{

};

struct TextureCreateInfo
{
};

class Device;
class Texture
{
public:
    Texture() = delete;
    Texture(Device* device, TextureCreateInfo info);
    virtual ~Texture() = default;

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Device* getDevice() const;

protected:
    Device* m_device{ nullptr };
};

} // namespace vkt