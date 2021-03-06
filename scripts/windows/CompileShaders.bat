if not exist "..\..\bin\shaders\vulkan\" mkdir ..\..\bin\shaders\vulkan

%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -fvk-use-dx-layout -fvk-invert-y -T vs_6_0 -E VS_Main -fvk-b-shift 0 0 -fvk-t-shift 4 0 -fvk-u-shift 12 0 ..\..\source\graphics\shaders\depth.hlsl -Fo ..\..\bin\shaders\vulkan\depth_VS.spv

%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -fvk-use-dx-layout -fvk-invert-y -T vs_6_0 -E VS_Common -fvk-t-shift 4 1 ..\..\source\graphics\shaders\gbuffercommon.hlsl -Fo ..\..\bin\shaders\vulkan\common_VS.spv
%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -fvk-use-dx-layout -fvk-use-dx-position-w -T ps_6_0 -E PS_Common -fvk-t-shift 4 1 ..\..\source\graphics\shaders\gbuffercommon.hlsl -Fo ..\..\bin\shaders\vulkan\common_PS.spv
%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -fvk-use-dx-layout -fvk-use-dx-position-w -T ps_6_0 -E PS_CommonNoNormals -fvk-t-shift 4 1 ..\..\source\graphics\shaders\gbuffercommon.hlsl -Fo ..\..\bin\shaders\vulkan\common_no_normals_PS.spv
%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -fvk-use-dx-layout -fvk-use-dx-position-w -T ps_6_0 -E PS_CommonDiffuseOnly -fvk-t-shift 4 1 ..\..\source\graphics\shaders\gbuffercommon.hlsl -Fo ..\..\bin\shaders\vulkan\common_diff_only_PS.spv
%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -fvk-use-dx-layout -fvk-use-dx-position-w -T ps_6_0 -E PS_GBufferTest ..\..\source\graphics\shaders\gbuffercommon.hlsl -Fo ..\..\bin\shaders\vulkan\gbuffer_test_PS.spv

%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -fvk-use-dx-layout -T cs_6_0 -E CS_ComputeCluster -fvk-b-shift 0 0 -fvk-t-shift 4 0 -fvk-u-shift 12 0 ..\..\source\graphics\shaders\clustercalculation.hlsl -Fo ..\..\bin\shaders\vulkan\clustercalculation_CS.spv
%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -fvk-use-dx-layout -fvk-invert-y -T vs_6_0 -E VS_MarkActiveClusters ..\..\source\graphics\shaders\clustervisibility.hlsl -Fo ..\..\bin\shaders\vulkan\markactiveclusters_VS.spv
%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -fvk-use-dx-layout -fvk-use-dx-position-w -T ps_6_0 -E PS_MarkActiveClusters -fvk-u-shift 12 0 ..\..\source\graphics\shaders\clustervisibility.hlsl -Fo ..\..\bin\shaders\vulkan\markactiveclusters_PS.spv
%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -fvk-use-dx-layout -T cs_6_0 -E CS_CompactActiveClusters -fvk-b-shift 0 0 -fvk-t-shift 4 0 -fvk-u-shift 12 0 ..\..\source\graphics\shaders\clustervisibility.hlsl -Fo ..\..\bin\shaders\vulkan\compactactiveclusters_CS.spv
%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -fvk-use-dx-layout -T cs_6_0 -E CS_CullLights -fvk-b-shift 0 0 -fvk-t-shift 4 0 -fvk-u-shift 12 0 ..\..\source\graphics\shaders\clusterlightculling.hlsl -Fo ..\..\bin\shaders\vulkan\clusterlightculling_CS.spv

%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -fvk-use-dx-layout -T cs_6_0 -E CS_SSAOCalculate -fvk-b-shift 0 0 -fvk-t-shift 4 0 -fvk-u-shift 12 0 ..\..\source\graphics\shaders\ssao.hlsl -Fo ..\..\bin\shaders\vulkan\ssao_calculate_CS.spv
%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -fvk-use-dx-layout -T cs_6_0 -E CS_SSAOBlur -fvk-b-shift 0 0 -fvk-t-shift 4 0 -fvk-u-shift 12 0 ..\..\source\graphics\shaders\ssao.hlsl -Fo ..\..\bin\shaders\vulkan\ssao_blur_CS.spv

%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -fvk-use-dx-layout -T cs_6_0 -E CS_DeferredLighting -fvk-b-shift 0 0 -fvk-t-shift 4 0 -fvk-u-shift 12 0 ..\..\source\graphics\shaders\deferredlighting.hlsl -Fo ..\..\bin\shaders\vulkan\deferredlighting_CS.spv
%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -fvk-use-dx-layout -T cs_6_0 -E CS_DeferredLighting -fvk-b-shift 0 0 -fvk-t-shift 4 0 -fvk-u-shift 12 0 ..\..\source\graphics\shaders\clusterdeferredlighting.hlsl -Fo ..\..\bin\shaders\vulkan\clusterdeferredlighting_CS.spv

%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -fvk-use-dx-layout -T cs_6_0 -E CS_ComputeHistogram -D COMPUTE_HISTOGRAM -fvk-b-shift 0 0 -fvk-t-shift 4 0 -fvk-u-shift 12 0 ..\..\source\graphics\shaders\hdr.hlsl -Fo ..\..\bin\shaders\vulkan\computehistogram_CS.spv
%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -fvk-use-dx-layout -T cs_6_0 -E CS_AverageHistogram -D AVERAGE_HISTOGRAM -fvk-b-shift 0 0 -fvk-t-shift 4 0 -fvk-u-shift 12 0 ..\..\source\graphics\shaders\hdr.hlsl -Fo ..\..\bin\shaders\vulkan\averagehistogram_CS.spv
%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -fvk-use-dx-layout -T cs_6_0 -E CS_Tonemap -D TONEMAP -fvk-b-shift 0 0 -fvk-t-shift 4 0 -fvk-u-shift 12 0 ..\..\source\graphics\shaders\hdr.hlsl -Fo ..\..\bin\shaders\vulkan\tonemap_CS.spv

%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -fvk-use-dx-layout -T cs_6_0 -E CS_FXAAFilter -fvk-b-shift 0 0 -fvk-t-shift 4 0 -fvk-u-shift 12 0 ..\..\source\graphics\shaders\fxaa.hlsl -Fo ..\..\bin\shaders\vulkan\fxaa_filter_CS.spv

pause