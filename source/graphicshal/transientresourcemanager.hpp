#pragma once
#include "core/primitivetypes.h"
#include "graphicshal/buffer.hpp"
#include "graphicshal/bufferview.hpp"
#include "graphicshal/config.h"
#include "graphicshal/texture.hpp"
#include "graphicshal/textureview.hpp"

// TODO: Hacky way to create a pair hash
struct pair_hash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& key) const {
        auto h1 = std::hash<T1>{}(key.first);
        auto h2 = std::hash<T2>{}(key.second);

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
        virtual RenderDevice& GetDevice() const = 0;

        virtual Buffer* FindOrCreateBuffer(const RenderPass* pass, const BufferDesc& desc, uint32 resourceIndex) = 0;
        virtual BufferView* FindOrCreateBufferView(Buffer* buffer, const BufferViewDesc& desc) = 0;
        virtual Texture* FindOrCreateTexture(const RenderPass* pass, const TextureDesc& desc, uint32 resourceIndex) = 0;
        virtual TextureView* FindOrCreateTextureView(Texture* texture, const TextureViewDesc& desc) = 0;

	protected:
        static constexpr uint32 K_BUFFER_POOL_SIZE = 32;
        static constexpr uint32 K_TEXTURE_POOL_SIZE = 32;
        static constexpr uint32 K_BUFFER_VIEW_POOL_SIZE = 256;
        static constexpr uint32 K_TEXTURE_VIEW_POOL_SIZE = 256;

        uint32 m_BufferPoolIndex = 0;
        uint32 m_BufferViewPoolIndex[K_MAX_FRAMES_IN_FLIGHT] = {};
        uint32 m_TexturePoolIndex = 0;
        uint32 m_TextureViewPoolIndex[K_MAX_FRAMES_IN_FLIGHT] = {};

        THashMap<TPair<const RenderPass*, BufferDesc>, TMap<uint32, Buffer*>, pair_hash> m_DescToBufferMap;
        THashMap<TPair<const RenderPass*, TextureDesc>, TMap<uint32, Texture*>, pair_hash> m_DescToTextureMap;
	};
}