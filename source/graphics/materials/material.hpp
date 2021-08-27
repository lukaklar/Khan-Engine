#pragma once

namespace Khan
{
	enum class BlendFactor;
	enum class BlendOperation;
	struct RenderPipelineState;
	class Shader;
	class TextureView;

	struct MaterialTexture
	{
		uint32_t m_Binding;
		TextureView* m_Texture;
	};

	class Material
	{
	public:
		inline bool IsCompiled() const { return m_PipelineState != nullptr; }
		inline RenderPipelineState* GetPipelineState() const { return m_PipelineState; }
		inline void SetPipelineState(RenderPipelineState* value) { m_PipelineState = value; }
		inline const Shader* GetPixelShader() const { return m_PixelShader; }
		inline void SetPixelShader(const Shader* pixelShader) { m_PixelShader = pixelShader; }
		inline const std::vector<MaterialTexture>& GetTextures() const { return m_Textures; }
		inline void AddTexture(uint32_t binding, TextureView* texture) { m_Textures.push_back({ binding, texture }); }
		inline bool HasTwoSides() const { return m_TwoSided; }
		inline void SetTwoSided(bool value) { m_TwoSided = value; }
		inline bool IsTransparent() const { return m_Transparent; }
		inline void SetTransparent(bool value) { m_Transparent = value; }
		inline BlendFactor GetSrcColorFactor() const { m_SrcColorFactor; }
		inline void SetSrcColorFactor(BlendFactor value) { m_SrcColorFactor = value; }
		inline BlendFactor GetDstColorFactor() const { m_DstColorFactor; }
		inline void SetDstColorFactor(BlendFactor value) { m_DstColorFactor = value; }
		inline BlendOperation GetColorBlendOp() const { return m_ColorBlendOp; }
		inline void SetColorBlendOp(BlendOperation value) { m_ColorBlendOp = value; }
		inline BlendFactor GetSrcAlphaFactor() const { m_SrcAlphaFactor; }
		inline void SetSrcAlphaFactor(BlendFactor value) { m_SrcAlphaFactor = value; }
		inline BlendFactor GetDstAlphaFactor() const { m_DstAlphaFactor; }
		inline void SetDstAlphaFactor(BlendFactor value) { m_DstAlphaFactor = value; }
		inline BlendOperation GetAlphaBlendOp() const { return m_AlphaBlendOp; }
		inline void SetAlphaBlendOp(BlendOperation value) { m_AlphaBlendOp = value; }

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