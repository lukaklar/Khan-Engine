#pragma once

namespace Khan
{
	class Buffer;
	class Material;

	struct SubMeshData
	{
		Material* m_Material;
		uint32_t m_VertexBufferOffset;
		uint32_t m_IndexBufferOffset;
		uint32_t m_NumIndices;
		bool m_TwoByteIndex;
	};

	class Mesh
	{
	public:

		inline Buffer* GetVertexBuffer() const { return m_VertexBuffer; }
		inline Buffer* GetIndexBuffer() const { return m_IndexBuffer; }
		inline const std::vector<SubMeshData>& GetSubMeshData() { return m_SubMeshData; }

	private:
		Buffer* m_VertexBuffer;
		Buffer* m_IndexBuffer;
		std::vector<SubMeshData> m_SubMeshData;
	};
}