#pragma once

namespace vkt
{

enum class TextureFormat
{

};

struct TextureDescriptor
{
};

class Device;
class Texture
{
public:
    Texture() = delete;
    Texture(Device* device, TextureDescriptor descriptor);
    virtual ~Texture() = default;

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Device* getDevice() const;

protected:
    Device* m_device{ nullptr };
};

} // namespace vkt
