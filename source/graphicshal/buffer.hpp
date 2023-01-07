#pragma once

#include "graphicshal/resource.hpp"

namespace Khan
{
	using BufferFlags = uint32_t;
	enum BufferFlag
	{
		BufferFlag_None						= 0x000,
		BufferFlag_Readable					= 0x001,
		BufferFlag_Writable					= 0x002,
		BufferFlag_AllowShaderResource		= 0x010,
		BufferFlag_AllowUnorderedAccess		= 0x020,
		BufferFlag_AllowIndirect			= 0x040,
		BufferFlag_AllowVertices			= 0x080,
		BufferFlag_AllowIndices				= 0x100
	};

	struct BufferDesc
	{
		uint32_t m_Size;
		BufferFlags m_Flags;

		inline bool operator==(const BufferDesc& other) const
		{
			return !std::memcmp(this, &other, sizeof(BufferDesc));
		}
	};

	class Buffer : public Resource
	{
	public:
		const BufferDesc& GetDesc() const { return m_Desc; }

	protected:
		Buffer(const BufferDesc& desc, ResourceState state = ResourceState_Undefined)
			: Resource(state)
			, m_Desc(desc)
		{
		}

		Buffer() = default;

		BufferDesc m_Desc;
	};
}

namespace std
{
	template <>
	struct hash<Khan::BufferDesc>
	{
		std::size_t operator()(const Khan::BufferDesc& key) const;
	};
}