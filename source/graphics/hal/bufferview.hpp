#pragma once

#include <cstdint>

namespace Khan
{
	enum PixelFormat;
	class Buffer;

	struct BufferViewDesc
	{
		uint32_t m_Offset;
		uint32_t m_Range;
		PixelFormat m_Format;

		inline bool operator==(const BufferViewDesc& other) const
		{
			return !std::memcmp(this, &other, sizeof(BufferViewDesc));
		}
	};

	class BufferView
	{
	public:
		Buffer& GetBuffer() { return *m_Buffer; }
		const Buffer& GetBuffer() const { return *m_Buffer; }
		const BufferViewDesc& GetDesc() const { return m_Desc; }

	protected:
		BufferView(Buffer& buffer, const BufferViewDesc& desc)
			: m_Buffer(&buffer)
			, m_Desc(desc)
		{
		}

		BufferView() = default;

		Buffer* m_Buffer;
		BufferViewDesc m_Desc;
	};
}

namespace std
{
	template <>
	struct hash<Khan::BufferViewDesc>
	{
		std::size_t operator()(const Khan::BufferViewDesc& key) const;
	};
}