#include "data/precomp.h"
#include "data/texturemanager.hpp"
#include "data/stb_image/stb_image.h"
#include "graphicshal/pixelformats.hpp"
#include "graphicshal/renderbackend.hpp"
#include "graphicshal/renderdevice.hpp"
#include "graphicshal/texture.hpp"
#include "graphicshal/textureview.hpp"

namespace Khan
{
	TextureManager::TextureManager()
	{
	}

	TextureManager::~TextureManager()
	{
		for (Texture* texture : m_Textures)
		{
			RenderBackend::g_Device->DestroyTexture(texture);
		}

		for (auto& it : m_TextureViewMap)
		{
			RenderBackend::g_Device->DestroyTextureView(it.second);
		}
	}

	TextureView* TextureManager::LoadTexture(const char* fileName)
	{
		auto it = m_TextureViewMap.find(fileName);
		if (it != m_TextureViewMap.end())
		{
			return it->second;
		}

		Texture* texture;

		{
			int32_t width, height, channels;
			unsigned char* data = stbi_load(fileName, &width, &height, &channels, STBI_rgb_alpha);

			TextureDesc desc;
			desc.m_Type = TextureType_2D;
			desc.m_Width = static_cast<uint32_t>(width);
			desc.m_Height = static_cast<uint32_t>(height);
			desc.m_Depth = 1;
			desc.m_ArrayLayers = 1;
			desc.m_MipLevels = 1;
			desc.m_Format = PF_R8G8B8A8_UNORM;
			desc.m_Flags = TextureFlag_AllowShaderResource | TextureFlag_Writable;

			texture = RenderBackend::g_Device->CreateTexture(desc, data);

			stbi_image_free(data);
		}

		m_Textures.push_back(texture);

		TextureView* view;

		{
			TextureViewDesc desc;
			desc.m_Type = TextureViewType_2D;
			desc.m_Format = PF_R8G8B8A8_UNORM;
			desc.m_BaseArrayLayer = 0;
			desc.m_LayerCount = 1;
			desc.m_BaseMipLevel = 0;
			desc.m_LevelCount = 1;

			view = RenderBackend::g_Device->CreateTextureView(texture, desc);
		}

		m_TextureViewMap.insert({ fileName, view });

		return view;
	}
}