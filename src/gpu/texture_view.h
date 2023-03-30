#pragma once

namespace vkt
{

class Texture;

struct TextureViewDescriptor
{
};

class TextureView
{
public:
    TextureView() = delete;
    TextureView(Texture* texture, TextureViewDescriptor descriptor);
    virtual ~TextureView() = default;

protected:
    Texture* m_texture{ nullptr };
};

} // namespace vkt
