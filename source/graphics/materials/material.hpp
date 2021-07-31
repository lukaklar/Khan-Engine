#pragma once

namespace Khan
{
	enum BlendFactor;
	enum BlendOperation;
	struct RenderPipelineState;
	class Shader;
	class TextureView;

	struct MaterialTexture
	{
		uint32_t m_Binding;
		TextureView* m_Texture;

		// TODO: Sampler state?
	};

	class Material
	{
	public:
		inline RenderPipelineState* GetPipelineState() const { return m_PipelineState; }
		inline void SetPipelineState(RenderPipelineState* value) { m_PipelineState = value; }
		inline bool IsCompiled() const { return m_PipelineState != nullptr; }
		inline const Shader* GetPixelShader() const { return m_PixelShader; }
		inline const std::vector<MaterialTexture>& GetTextures() const { return m_Textures; }
		inline bool HasTwoSides() const { return m_TwoSided; }
		inline bool IsTransparent() const { return m_Transparent; }
		inline BlendFactor GetSrcColorFactor() const { m_SrcColorFactor; }
		inline BlendFactor GetDstColorFactor() const { m_DstColorFactor; }
		inline BlendOperation GetColorBlendOp() const { return m_ColorBlendOp; }
		inline BlendFactor GetSrcAlphaFactor() const { m_SrcAlphaFactor; }
		inline BlendFactor GetDstAlphaFactor() const { m_DstAlphaFactor; }
		inline BlendOperation GetAlphaBlendOp() const { return m_AlphaBlendOp; }

	private:
		const Shader* m_PixelShader;
		std::vector<MaterialTexture> m_Textures;
		bool m_TwoSided;
		bool m_Transparent;
		BlendFactor m_SrcColorFactor;
		BlendFactor m_DstColorFactor;
		BlendOperation m_ColorBlendOp;
		BlendFactor m_SrcAlphaFactor;
		BlendFactor m_DstAlphaFactor;
		BlendOperation m_AlphaBlendOp;

		RenderPipelineState* m_PipelineState = nullptr;
	};
}