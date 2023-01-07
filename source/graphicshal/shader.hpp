#pragma once

namespace Khan
{
	enum ShaderType
	{
		ShaderType_Vertex,
		ShaderType_Domain,
		ShaderType_Hull,
		ShaderType_Geometry,
		ShaderType_Pixel,
		ShaderType_Compute,
		ShaderType_Count
	};

	struct ShaderDesc
	{
		ShaderType m_Type;
		const void* m_Bytecode;
		uint32_t m_BytecodeSize;
		const char* m_EntryPoint;
	};

	class Shader
	{
	public:
		ShaderType GetType() const { return m_Type; }

#ifdef KH_DEBUG
		const std::string& GetDebugName() const { return m_DebugName; }
		void SetDebugName(const std::string& debugName) { m_DebugName = debugName; }
#endif // KH_DEBUG

	protected:
		Shader(ShaderType type)
			: m_Type(type)
		{
		}

		virtual ~Shader() = 0;

		ShaderType m_Type;

#ifdef KH_DEBUG
		std::string m_DebugName;
#endif // KH_DEBUG
	};
}