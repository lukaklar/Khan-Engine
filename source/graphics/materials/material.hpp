#pragma once

namespace Khan
{
	class RenderPipelineState;
	class Shader;

	class Material
	{
	public:


		inline RenderPipelineState* GetPipelineState() const { return m_PipelineState; }
		inline void SetPipelineState(RenderPipelineState* value) { m_PipelineState = value; }
		inline bool IsCompiled() const { return m_PipelineState != nullptr; }
		inline bool HasTwoSides() const { return m_TwoSided; }
		inline bool IsTransparent() const { return m_Transparent; }

	private:
		const Shader* m_PixelShader;
		bool m_Transparent;
		bool m_TwoSided;

		RenderPipelineState* m_PipelineState = nullptr;
	};
}