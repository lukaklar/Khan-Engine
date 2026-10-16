#pragma once
#include "core/primitivetypes.h"

namespace Khan
{
	struct DepthStencilState
	{
		enum class CompareFunction
		{
			Never,
			Less,
			Equal,
			LessOrEqual,
			Greater,
			NotEqual,
			GreaterOrEqual,
			Always
		};

		enum class StencilOperation
		{
			Keep,
			Zero,
			Replace,
			IncrementClamp,
			DecrementClamp,
			Invert,
			IncrementWrap,
			DecrementWrap
		};

		struct DepthMode
		{
			bool m_DepthTestEnabled = false;
			bool m_DepthWriteEnabled = false;
			bool m_DepthBoundsTestEnabled = false;
			CompareFunction m_DepthFunc = CompareFunction::Never;
			float m_MinDepthBound = 0.0f;
			float m_MaxDepthBound = 0.0f;
		};

		struct FaceOperation
		{
			StencilOperation m_StencilFailOp = StencilOperation::Keep;
			StencilOperation m_StencilPassOp = StencilOperation::Keep;
			StencilOperation m_DepthFailOp = StencilOperation::Keep;
			CompareFunction m_StencilFunc = CompareFunction::Never;
		};

		struct StencilMode
		{
			bool m_StencilTestEnabled = false;
			uint8 m_ReadMask = 0xff;
			uint8 m_WriteMask = 0xff;
			uint32 m_StencilReference = 0;
			FaceOperation m_FrontFace;
			FaceOperation m_BackFace;
		};

		DepthMode m_DepthMode;
		StencilMode m_StencilMode;
	};
}