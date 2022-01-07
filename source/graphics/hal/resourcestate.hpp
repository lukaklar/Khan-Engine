#pragma once

namespace Khan
{
    enum ResourceState
    {
        ResourceState_Undefined,
        ResourceState_General,
        ResourceState_CopySource,
        ResourceState_CopyDestination,
        ResourceState_VertexBuffer,
        ResourceState_IndexBuffer,
        ResourceState_IndirectArgument,
        ResourceState_ConstantBuffer,
        ResourceState_UnorderedAccess,
        ResourceState_NonPixelShaderAccess,
        ResourceState_PixelShaderAccess,
        ResourceState_PixelShaderWrite,
        ResourceState_AnyShaderAccess,
        ResourceState_StreamOut,
        ResourceState_RenderTarget,
        ResourceState_DepthWrite,
        ResourceState_DepthRead,
        ResourceState_StencilWrite,
        ResourceState_StencilRead,
        ResourceState_DepthWriteStencilWrite,
        ResourceState_DepthReadStencilRead,
        ResourceState_DepthReadStencilWrite,
        ResourceState_DepthWriteStencilRead,
        ResourceState_Present,
        ResourceState_Count
    };
}