#pragma once

#include <cstdint>

namespace Khan
{
	enum PixelFormat;
	class Texture;

	enum TextureViewType
	{
		TextureViewType_1D,
		TextureViewType_2D,
		TextureViewType_3D,
		TextureViewType_Cube,
		TextureViewType_1DArray,
		TextureViewType_2DArray,
		TextureViewType_CubeArray
	};

	struct TextureViewDesc
	{
		TextureViewType m_Type;
		PixelFormat m_Format;
		uint32_t m_BaseArrayLayer;
		uint32_t m_LayerCount;
		uint32_t m_BaseMipLevel;
		uint32_t m_LevelCount;

		inline bool operator==(const TextureViewDesc& other) const
		{
			return !std::memcmp(this, &other, sizeof(TextureViewDesc));
		}
	};

	class TextureView
	{
	public:
		Texture& GetTexture() { return *m_Texture; }
		const Texture& GetTexture() const { return *m_Texture; }
		const TextureViewDesc& GetDesc() const { return m_Desc; }

	protected:
		TextureView(Texture& texture, const TextureViewDesc& desc)
			: m_Texture(&texture)
			, m_Desc(desc)
		{
		}

		TextureView() = default;

		Texture* m_Texture;
		TextureViewDesc m_Desc;
	};
}

namespace std
{
	template <>
	struct hash<Khan::TextureViewDesc>
	{
		std::size_t operator()(const Khan::TextureViewDesc& key) const;
	};
}