#include "graphics/precomp.h"
#include "graphics/hal/vertexinputstate.hpp"

namespace Khan
{
	const char* VertexInputState::StreamDescriptor::StreamElement::GetSemanticName() const
	{
		switch (m_Usage)
		{
		case Usage::POSITION:
			return "POSITION";
		case Usage::NORMAL:
			return "NORMAL";
		case Usage::TANGENT:
			return "TANGENT";
		case Usage::BITANGENT:
			return "BITANGENT";
		case Usage::COLOR0:
		case Usage::COLOR1:
			return "COLOR";
		case Usage::TEXCOORD0:
		case Usage::TEXCOORD1:
		case Usage::TEXCOORD2:
			return "TEXCOORD";
		default:
			return "";
		}
	}

	uint32_t VertexInputState::StreamDescriptor::StreamElement::GetSemanticIndex() const
	{
		switch (m_Usage)
		{
		case Usage::POSITION:
		case Usage::NORMAL:
		case Usage::TANGENT:
		case Usage::BITANGENT:
		case Usage::COLOR0:
		case Usage::TEXCOORD0:
			return 0;
		case Usage::COLOR1:
		case Usage::TEXCOORD1:
			return 1;
		case Usage::TEXCOORD2:
			return 2;
		default:
			return -1;
		}
	}

	uint32_t VertexInputState::StreamDescriptor::StreamElement::GetSize() const
	{
		switch (m_Type)
		{
		case Type::Float:
		case Type::Int:
		case Type::Uint:
			return 4;
		case Type::Float2:
		case Type::Int2:
		case Type::Uint2:
			return 8;
		case Type::Float3:
		case Type::Int3:
		case Type::Uint3:
			return 12;
		case Type::Float4:
		case Type::Int4:
		case Type::Uint4:
			return 16;
		default:
			return 0;
		}
	}

	void VertexInputState::StreamDescriptor::AddStreamElement(StreamElement::Type type, StreamElement::Usage usage)
	{
		m_StreamElements[m_NumElements].m_Type = type;
		m_StreamElements[m_NumElements].m_Usage = usage;
		m_StreamElements[m_NumElements].m_Offset = m_NumElements == 0 ? 0 : m_StreamElements[m_NumElements - 1].GetOffset() + m_StreamElements[m_NumElements].GetSize();
		m_Stride += m_StreamElements[m_NumElements].GetSize();
		++m_NumElements;
	}

	void VertexInputState::AddStreamElement(uint32_t stream, StreamDescriptor::StreamElement::Type type, StreamDescriptor::StreamElement::Usage usage)
	{
		m_StreamDescriptors[stream].AddStreamElement(type, usage);
		m_ActiveStreamMask |= 1 << stream;
	}
}