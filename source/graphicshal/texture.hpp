#pragma once

#include "graphicshal/resource.hpp"

namespace Khan
{
	enum PixelFormat;

	using TextureFlags = uint32_t;
	enum TextureFlag
	{
		TextureFlag_None					= 0x000,
		//TextureFlag_Dynamic					= 0x001,
		//TextureFlag_Staging					= 0x002,
		TextureFlag_Readable				= 0x004,
		TextureFlag_Writable				= 0x008,
		TextureFlag_AllowShaderResource		= 0x010,
		TextureFlag_AllowUnorderedAccess	= 0x020,
		TextureFlag_AllowRenderTarget		= 0x040,
		TextureFlag_AllowDepthStencil		= 0x080,
		TextureFlag_CubeMap					= 0x100
	};

	enum TextureType
	{
		TextureType_1D,
		TextureType_2D,
		TextureType_3D
	};

	struct TextureDesc
	{
		TextureType m_Type;
		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_Depth;
		uint32_t m_ArrayLayers;
		uint32_t m_MipLevels;
		//uint32_t m_SampleCount;
		PixelFormat m_Format;
		TextureFlags m_Flags;

		inline bool operator==(const TextureDesc& other) const
		{
			return !std::memcmp(this, &other, sizeof(TextureDesc));
		}
	};

	class Texture : public Resource
	{
	public:
		const TextureDesc& GetDesc() const { return m_Desc; }

	protected:
		Texture(const TextureDesc& desc, ResourceState state = ResourceState_Undefined)
			: Resource(state)
			, m_Desc(desc)
		{
		}

		Texture() = default;

		TextureDesc m_Desc;
	};
}

namespace std
{
	template <>
	struct hash<Khan::TextureDesc>
	{
		std::size_t operator()(const Khan::TextureDesc& key) const;
	};
}