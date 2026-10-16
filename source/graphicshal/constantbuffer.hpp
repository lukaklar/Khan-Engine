#pragma once
#include "core/primitivetypes.h"

namespace Khan
{
	class ConstantBuffer
	{
	public:
		ConstantBuffer(uint32 size);
		~ConstantBuffer();

		void UpdateConstantData(const void* data, uint32 offset, uint32 size);
		const void* Data() const;
		uint32 Size() const;
		uint32 GetDynamicOffset() const;
		void SetDynamicOffset(uint32 offset);
		bool IsDirty() const;

	private:
		byte* m_ShadowData;
		uint32 m_Size;
		uint32 m_DynamicOffset;
		bool m_IsDirty;
	};
}

#include "graphicshal/constantbuffer.inl"