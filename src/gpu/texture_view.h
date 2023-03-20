#pragma once

namespace vkt
{

class Texture;

struct TextureViewCreateInfo
{
};

class TextureView
{
public:
    TextureView() = delete;
    TextureView(Texture* texture, TextureViewCreateInfo info);
    virtual ~TextureView() = default;

protected:
    Texture* m_texture{ nullptr };
};

} // namespace vkt