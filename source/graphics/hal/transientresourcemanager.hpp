#pragma once

#include "graphics/hal/buffer.hpp"
#include "graphics/hal/bufferview.hpp"
#include "graphics/hal/texture.hpp"
#include "graphics/hal/textureview.hpp"

// TODO: Hacky way to create a pair hash
struct pair_hash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& key) const {
        auto h1 = std::hash<T1>{}(key.first);
        auto h2 = std::hash<T2>{}(key.second);

        return h1 ^ h2;
    }
};

struct double_pair_hash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& key) const {
        auto h1 = std::hash<T1>{}(key.first);
        auto h2 = pair_hash{}(key.second);

        return h1 ^ h2;
    }
};

namespace Khan
{
    class RenderDevice;
    class RenderPass;

	class TransientResourceManager
	{
	public:
        TransientResourceManager(RenderDevice& device)
            : m_Device(device)
        {
        }

        virtual Buffer* FindOrCreateBuffer(const RenderPass* pass, const BufferDesc& desc) = 0;
        virtual BufferView* FindOrCreateBufferView(const RenderPass* pass, Buffer* buffer, const BufferViewDesc& desc) = 0;
        virtual Texture* FindOrCreateTexture(const RenderPass* pass, const TextureDesc& desc) = 0;
        virtual TextureView* FindOrCreateTextureView(const RenderPass* pass, Texture* texture, const TextureViewDesc& desc) = 0;

        inline RenderDevice& GetDevice() const { return m_Device; }

	protected:
        static constexpr uint32_t K_BUFFER_POOL_SIZE = 32;
        static constexpr uint32_t K_TEXTURE_POOL_SIZE = 32;
        static constexpr uint32_t K_BUFFER_VIEW_POOL_SIZE = 256;
        static constexpr uint32_t K_TEXTURE_VIEW_POOL_SIZE = 256;

        uint32_t m_BufferPoolIndex = 0;
        uint32_t m_BufferViewPoolIndex[K_MAX_FRAMES_IN_FLIGHT] = {};
        uint32_t m_TexturePoolIndex = 0;
        uint32_t m_TextureViewPoolIndex[K_MAX_FRAMES_IN_FLIGHT] = {};

        std::unordered_map<std::pair<const RenderPass*, BufferDesc>, Buffer*, pair_hash> m_DescToBufferMap;
        std::unordered_map<std::pair<const RenderPass*, TextureDesc>, Texture*, pair_hash> m_DescToTextureMap;
        

        RenderDevice& m_Device;
	};
}