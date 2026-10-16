#pragma once
#include "core/primitivetypes.h"
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
				enum class Type : uint8
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

				enum class Usage : uint8
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
				uint32 GetSemanticIndex() const;
				uint32 GetSize() const;
				uint8 GetOffset() const { return m_Offset; }

			private:
				friend StreamDescriptor;

				Type m_Type;
				Usage m_Usage;
				uint8 m_Offset;
			};

			void AddStreamElement(StreamElement::Type type, StreamElement::Usage usage);
			inline uint32 GetNumElements() const { return m_NumElements; }
			inline const StreamElement* GetStreamElements() const { return m_StreamElements; }
			inline uint8 GetStride() const { return m_Stride; }

		private:
			StreamElement m_StreamElements[K_MAX_STREAM_ELEMENTS] = {};
			uint8 m_NumElements = 0;
			uint8 m_Stride = 0;
		};

		void AddStreamElement(uint32 stream, StreamDescriptor::StreamElement::Type type, StreamDescriptor::StreamElement::Usage usage);
		inline const StreamDescriptor* GetStreamDescriptors() const { return m_StreamDescriptors; }
		inline uint16 GetActiveStreamMask() const { return m_ActiveStreamMask; }
		inline void SetStreamInstanced(uint32 stream) { m_InstancedStreamMask |= 1 << stream; }
		inline uint16 GetInstancedStreamMask() const { return m_InstancedStreamMask; }

	private:
		StreamDescriptor m_StreamDescriptors[K_MAX_VERTEX_DATA_STREAMS] = {};
		uint16 m_ActiveStreamMask = 0;
		uint16 m_InstancedStreamMask = 0;
	};
}