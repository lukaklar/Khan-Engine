#pragma once

#include "graphicshal/config.h"

namespace Khan
{
	class VertexInputState
	{
	public:
		class StreamDescriptor
		{
		public:
			class StreamElement
			{
			public:
				enum class Type : uint8_t
				{
					Float,
					Float2,
					Float3,
					Float4,
					Int,
					Int2,
					Int3,
					Int4,
					Uint,
					Uint2,
					Uint3,
					Uint4
				};

				enum class Usage : uint8_t
				{
					POSITION,
					NORMAL,
					TANGENT,
					BITANGENT,
					COLOR0,
					COLOR1,
					TEXCOORD0,
					TEXCOORD1,
					TEXCOORD2
				};

				Type GetType() const { return m_Type; }
				const char* GetSemanticName() const;
				uint32_t GetSemanticIndex() const;
				uint32_t GetSize() const;
				uint8_t GetOffset() const { return m_Offset; }

			private:
				friend StreamDescriptor;

				Type m_Type;
				Usage m_Usage;
				uint8_t m_Offset;
			};

			void AddStreamElement(StreamElement::Type type, StreamElement::Usage usage);
			inline uint32_t GetNumElements() const { return m_NumElements; }
			inline const StreamElement* GetStreamElements() const { return m_StreamElements; }
			inline uint8_t GetStride() const { return m_Stride; }

		private:
			StreamElement m_StreamElements[K_MAX_STREAM_ELEMENTS] = {};
			uint8_t m_NumElements = 0;
			uint8_t m_Stride = 0;
		};

		void AddStreamElement(uint32_t stream, StreamDescriptor::StreamElement::Type type, StreamDescriptor::StreamElement::Usage usage);
		inline const StreamDescriptor* GetStreamDescriptors() const { return m_StreamDescriptors; }
		inline uint16_t GetActiveStreamMask() const { return m_ActiveStreamMask; }
		inline void SetStreamInstanced(uint32_t stream) { m_InstancedStreamMask |= 1 << stream; }
		inline uint16_t GetInstancedStreamMask() const { return m_InstancedStreamMask; }

	private:
		StreamDescriptor m_StreamDescriptors[K_MAX_VERTEX_DATA_STREAMS] = {};
		uint16_t m_ActiveStreamMask = 0;
		uint16_t m_InstancedStreamMask = 0;
	};
}