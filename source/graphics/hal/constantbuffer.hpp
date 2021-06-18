#pragma once

namespace Khan
{
	class ConstantBuffer
	{
	public:
		ConstantBuffer(uint32_t size);
		~ConstantBuffer();

		void UpdateConstantData(const void* data, uint32_t offset, uint32_t size);
		const void* Data() const;
		uint32_t Size() const;
		uint32_t GetDynamicOffset() const;
		void SetDynamicOffset(uint32_t offset);
		bool IsDirty() const;

	private:
		uint8_t* m_ShadowData;
		uint32_t m_Size;
		uint32_t m_DynamicOffset;
		bool m_IsDirty;
	};
}

#include "graphics/hal/constantbuffer.inl"