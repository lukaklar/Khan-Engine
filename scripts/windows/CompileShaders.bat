if not exist "..\..\bin\shaders\vulkan\" mkdir ..\..\bin\shaders\vulkan

%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -fvk-use-dx-layout -fvk-invert-y -T vs_6_0 -E VS_Test ..\..\source\graphics\shaders\test.hlsl -Fo ..\..\bin\shaders\vulkan\test_VS.spv
%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -fvk-use-dx-position-w -T ps_6_0 -E PS_Test ..\..\source\graphics\shaders\test.hlsl -Fo ..\..\bin\shaders\vulkan\test_PS.spv

%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -fvk-use-dx-layout -fvk-invert-y -T vs_6_0 -E VS_Common -fvk-t-shift 4 1 ..\..\source\graphics\shaders\gbuffercommon.hlsl -Fo ..\..\bin\shaders\vulkan\common_VS.spv
%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -fvk-use-dx-layout -fvk-use-dx-position-w -T ps_6_0 -E PS_Common -fvk-t-shift 4 1 ..\..\source\graphics\shaders\gbuffercommon.hlsl -Fo ..\..\bin\shaders\vulkan\common_PS.spv
%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -fvk-use-dx-layout -fvk-use-dx-position-w -T ps_6_0 -E PS_CommonNoNormals -fvk-t-shift 4 1 ..\..\source\graphics\shaders\gbuffercommon.hlsl -Fo ..\..\bin\shaders\vulkan\common_no_normals_PS.spv
%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -fvk-use-dx-layout -fvk-use-dx-position-w -T ps_6_0 -E PS_CommonDiffuseOnly -fvk-t-shift 4 1 ..\..\source\graphics\shaders\gbuffercommon.hlsl -Fo ..\..\bin\shaders\vulkan\common_diff_only_PS.spv
%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -fvk-use-dx-layout -fvk-use-dx-position-w -T ps_6_0 -E PS_GBufferTest ..\..\source\graphics\shaders\gbuffercommon.hlsl -Fo ..\..\bin\shaders\vulkan\gbuffer_test_PS.spv

%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -fvk-use-dx-layout -fvk-invert-y -T vs_6_0 -E VS_Main -fvk-b-shift 0 0 -fvk-t-shift 4 0 -fvk-u-shift 12 0 ..\..\source\graphics\shaders\depth.hlsl -Fo ..\..\bin\shaders\vulkan\depth_VS.spv

%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -fvk-use-dx-layout -T cs_6_0 -E CS_ComputeFrustums -fvk-b-shift 0 0 -fvk-t-shift 4 0 -fvk-u-shift 12 0 ..\..\source\graphics\shaders\tilefrustumcalculation.hlsl -Fo ..\..\bin\shaders\vulkan\tilefrustumcalculation_CS.spv

%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -fvk-use-dx-layout -T cs_6_0 -E CS_CullLights -fvk-b-shift 0 0 -fvk-t-shift 4 0 -fvk-u-shift 12 0 ..\..\source\graphics\shaders\tileddeferredculling.hlsl -Fo ..\..\bin\shaders\vulkan\tileddeferredculling_CS.spv

%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -fvk-use-dx-layout -T cs_6_0 -E CS_TiledDeferredLighting -fvk-b-shift 0 0 -fvk-t-shift 4 0 -fvk-u-shift 12 0 ..\..\source\graphics\shaders\tileddeferredlighting.hlsl -Fo ..\..\bin\shaders\vulkan\tileddeferredlighting_CS.spv

%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -T cs_6_0 -E CS_DownScalePass1 -fvk-b-shift 0 0 -fvk-t-shift 4 0 -fvk-u-shift 12 0 ..\..\source\graphics\shaders\hdr.hlsl -Fo ..\..\bin\shaders\vulkan\DownScalePass1_CS.spv
%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -T cs_6_0 -E CS_DownScalePass2 -fvk-b-shift 0 0 -fvk-t-shift 4 0 -fvk-u-shift 12 0 ..\..\source\graphics\shaders\hdr.hlsl -Fo ..\..\bin\shaders\vulkan\DownScalePass2_CS.spv
%~dp0..\..\extern\VulkanSDK\Windows\Bin\dxc.exe -spirv -T cs_6_0 -E CS_TonemapPass -fvk-b-shift 0 0 -fvk-t-shift 4 0 -fvk-u-shift 12 0 ..\..\source\graphics\shaders\hdr.hlsl -Fo ..\..\bin\shaders\vulkan\TonemapPass_CS.spv

pause