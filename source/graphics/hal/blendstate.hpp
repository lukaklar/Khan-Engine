#pragma once
#include "graphics/hal/config.h"

namespace Khan
{
    struct BlendState
    {
        enum class BlendOperation
        {
            Add,
            Subtract,
            ReverseSubtract,
            Min,
            Max
        };

        enum class BlendFactor
        {
            Zero,
            One,
            SrcColor,
            OneMinusSrcColor,
            DstColor,
            OneMinusDstColor,
            SrcAlpha,
            OneMinusSrcAlpha,
            DstAlpha,
            OneMinusDstAlpha,
            SrcAlphaSaturate,
            Src1Color,
            OneMinusSrc1Color,
            Src1Alpha,
            OneMinusSrc1Alpha,
            ConstFactor,
            OneMinusConstFactor
        };

        enum class LogicOperator
        {
            None,
            Clear,
            Set,
            Copy,
            CopyInverted,
            Invert,
            And,
            Nand,
            AndReverse,
            AndInverted,
            Or,
            Nor,
            OrReverse,
            OrInverted,
            Xor,
            Equivalent
        };

        enum class ColorWriteEnable
        {
            Red     = 0x01,
            Green   = 0x02,
            Blue    = 0x04,
            Alpha   = 0x08,
            All     = 0x0F
        };

        struct BlendMode
        {

            BlendFactor m_SrcColorFactor = BlendFactor::One;
            BlendFactor m_DstColorFactor = BlendFactor::Zero;
            BlendOperation m_ColorBlendOp = BlendOperation::Add;
            BlendFactor m_SrcAlphaFactor = BlendFactor::One;
            BlendFactor m_DstAlphaFactor = BlendFactor::Zero;
            BlendOperation m_AlphaBlendOp = BlendOperation::Add;
            LogicOperator m_LogicOperator = LogicOperator::None;
            bool m_BlendEnabled = false;
            bool m_LogicOpEnabled = false;
            uint8_t m_ColorWriteMask = static_cast<uint8_t>(ColorWriteEnable::All);
        };

        BlendMode m_BlendModes[K_MAX_RENDER_TARGETS];
        //glm::vec4 m_ConstantBlendFactor;
        bool m_IndependentBlendEnabled = false;
        bool m_AlphaToCoverageEnabled = false;
    };
}