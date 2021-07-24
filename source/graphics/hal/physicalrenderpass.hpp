#pragma once

#include "graphics/hal/config.h"
#include "graphics/hal/pixelformats.hpp"
#include "system/assert.h"
#include <thirdparty/glm/glm.hpp>

namespace Khan
{
	enum class StartAccessType
	{
		Discard,
		Keep,
		Clear
	};

	enum class EndAccessType
	{
		Discard,
		Keep
	};

	struct PhysicalRenderPassDescription
	{
		uint32_t m_RenderTargetCount = 0;

		struct
		{
			PixelFormat m_Format = PF_NONE;
			StartAccessType m_StartAccess = StartAccessType::Discard;
			EndAccessType m_EndAccess = EndAccessType::Discard;
		} m_RenderTargets[K_MAX_RENDER_TARGETS];

		struct
		{
			PixelFormat m_Format = PF_NONE;
			StartAccessType m_DepthStartAccess = StartAccessType::Discard;
			EndAccessType m_DepthEndAccess = EndAccessType::Discard;
			StartAccessType m_StencilStartAccess = StartAccessType::Discard;
			EndAccessType m_StencilEndAccess = EndAccessType::Discard;
		} m_DepthStencil;

		bool operator==(const PhysicalRenderPassDescription& other) const
		{
			return !std::memcmp(this, &other, sizeof(PhysicalRenderPassDescription));
		}
	};

	class PhysicalRenderPass
	{
	public:
		const PhysicalRenderPassDescription& GetDesc() const { return m_Desc; }

		inline const glm::vec4& GetRenderTargetClearColor(uint32_t slot) const { KH_ASSERT(slot < K_MAX_RENDER_TARGETS, "Render target slot out of bounds."); return m_RenderTargetClearColors[slot]; }
		inline void SetRenderTargetClearColor(uint32_t slot, float r, float g, float b, float a) { KH_ASSERT(slot < K_MAX_RENDER_TARGETS, "Render target slot out of bounds."); m_RenderTargetClearColors[slot] = { r, g, b, a }; }
		float GetDepthClearValue() const { return m_DepthClearValue; }
		void SetDepthClearValue(float value) { m_DepthClearValue = value; }
		uint32_t GetStencilClearValue() const { return m_StencilClearValue; }
		void SetStencilClearValue(uint32_t value) { m_StencilClearValue = value; }

	protected:
		PhysicalRenderPass(const PhysicalRenderPassDescription& desc)
			:  m_Desc(desc)
		{
		}

		virtual ~PhysicalRenderPass() = 0;

		PhysicalRenderPassDescription m_Desc;

		glm::vec4 m_RenderTargetClearColors[K_MAX_RENDER_TARGETS] = {};
		float m_DepthClearValue = 1.0f;
		uint32_t m_StencilClearValue = 0;
	};
}

namespace std
{
	template <>
	struct hash<Khan::PhysicalRenderPassDescription>
	{
		std::size_t operator()(const Khan::PhysicalRenderPassDescription& key) const;
	};
}