#pragma once

#include "gpu/texture.h"
#include "gpu/texture_view.h"

#include <memory>
#include <vector>

namespace vkt
{

class Device;
class Surface;

struct SwapChainDescriptor
{
    std::unique_ptr<Surface> surface{ nullptr };
};

class SwapChain
{
public:
    SwapChain() = delete;
    SwapChain(Device* device, SwapChainDescriptor descriptor) noexcept;
    virtual ~SwapChain() noexcept = default;

    SwapChain(const SwapChain&) = delete;
    SwapChain& operator=(const SwapChain&) = delete;

    std::vector<Texture*> getTextures() const;
    std::vector<TextureView*> getTextureViews() const;

protected:
    Device* m_device{ nullptr };
    std::unique_ptr<Surface> m_surface{ nullptr };

    std::vector<std::unique_ptr<Texture>> m_textures{};
    std::vector<std::unique_ptr<TextureView>> m_textureViews{};
};

} // namespace vkt
